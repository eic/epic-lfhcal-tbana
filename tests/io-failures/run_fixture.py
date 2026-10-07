#!/usr/bin/env python3
"""Build real CLI entry points with fault-injected Analyses methods and real ROOT.

Arguments: --source NewStructure --baseline baseline-NewStructure --root ROOT-prefix
--out isolated-test-directory. Baseline optional. Requires a C++ compiler/rootcling.
Does not exercise the decoder, detector event loop, or MIP fitting routines.
"""
import argparse
import json
import os
from pathlib import Path
import shlex
import shutil
import subprocess

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--source', type=Path, required=True)
p.add_argument('--baseline', type=Path)
p.add_argument('--root', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
a=p.parse_args()
a.source=a.source.resolve(); a.root=a.root.resolve(); a.out=a.out.resolve()
a.out.mkdir(parents=True, exist_ok=True)
config=a.root/'bin/root-config'
flags=shlex.split(subprocess.check_output([str(config),'--cflags','--libs'],text=True).replace('/mock_site_packages/ROOT',''))
flags += ['-Wl,-rpath,'+str(a.root/'lib')]
bundled = a.root.parent/'root.libs'
if bundled.is_dir():
    flags += ['-Wl,-rpath-link,'+str(bundled), '-Wl,-rpath,'+str(bundled)]
    os.environ['LD_LIBRARY_PATH'] = str(bundled)+':'+str(a.root/'lib')+':'+os.environ.get('LD_LIBRARY_PATH','')
fixture=Path(__file__).resolve().parent/'FaultAnalyses.cc'
core=['Setup','RootSetupWrapper','Calib','Event','Tile','HGCROC','Caen']
linkdef='''#ifdef __CLING__
#pragma link C++ class Setup+;
#pragma link C++ class RootSetupWrapper+;
#pragma link C++ class Calib+;
#pragma link C++ struct TileCalib+;
#pragma link C++ class Event+;
#pragma link C++ class Tile+;
#pragma link C++ class Hgcroc+;
#pragma link C++ class Caen+;
#endif
'''
versions=[('after',a.source)]
if a.baseline: versions.insert(0,('before',a.baseline.resolve()))
rows=[]
for label, source in versions:
    work=a.out/label
    work.mkdir(exist_ok=True)
    (work/'LinkDef.h').write_text(linkdef)
    for ext in ('.h','.cc'):
        for stem in core:
            origin=source/(stem+ext)
            if not origin.exists(): origin=a.source/(stem+ext)
            shutil.copy2(origin,work/origin.name)
    for name in ('DataPrep.cc','Convert.cc','Analyses.h'):
        origin=source/name
        if not origin.exists(): origin=a.source/name
        shutil.copy2(origin,work/name)
    if (source/'CheckedIO.h').exists(): shutil.copy2(source/'CheckedIO.h',work/'CheckedIO.h')
    dictionary=work/'Cint.cc'
    subprocess.run([str(a.root/'bin/rootcling'),'-f',str(dictionary),'-I'+str(work),
                    *[str(work/(s+'.h')) for s in core],str(work/'LinkDef.h')],check=True)
    library=work/'libFixture.so'
    subprocess.run(['g++','-shared','-fPIC','-O0','-I'+str(work),
                    *[str(work/(s+'.cc')) for s in core],str(dictionary),'-o',str(library),*flags],check=True)
    for executable in ('DataPrep','Convert'):
        binary=work/executable
        subprocess.run(['g++','-O0','-I'+str(work),str(work/(executable+'.cc')),str(fixture),
                        '-L'+str(work),'-lFixture','-Wl,-rpath,'+str(work),'-o',str(binary),*flags],check=True)
        for mode in ('good','check_false','process_false','root_write','root_short','root_close',
                     'text_full','text_open','image','fit_error','warning'):
            output=work/(executable+'-'+mode+'.root')
            run=subprocess.run([str(binary),'-i','fixture','-o',str(output)],cwd=work,
                env=dict(os.environ,LFHCAL_IO_TEST=mode),capture_output=True,text=True)
            (work/(executable+'-'+mode+'.log')).write_text(run.stdout+run.stderr)
            expected=0 if mode in ('good','fit_error','warning') else 1
            rows.append(dict(version=label,executable=executable,case=mode,
                             exit=run.returncode,expected_after=expected))
            print(label,executable,mode,'exit='+str(run.returncode),flush=True)
            if label=='after': assert run.returncode == expected, rows[-1]
(a.out/'results.json').write_text(json.dumps(rows,indent=2)+'\n')
print('Passed: patched CLIs reject injected failures and accept healthy output, warnings, and fit-only errors.')
