Added Clustering functions

Do_Clustering_Window and DoClustering_Seeded_3D to Commonhelperfunctions.h

Window clustering scans across a 2D projection to find the window of size (windowsize) with the highest energy

Seeded clusters in cells around the highest energy cell above a provided seed energy value

These take in 3D histograms (to be updated to just the cells) and performing clutering, taking the seeded approach as nominal for now.  It returns a vector of Cluster objects.  The Cluster Object has the following properties saved out:

    int x;    	 	    energy weighted x center of the cluster
    int y;   		    energy weighted y center of the cluster
  int highestx;	    	    x location of highest energy tower in cluster
  int highesty;	      	    y location of highest energy tower in cluster
  int highestz;	      	    z location of highest energy z slice in cluster
    int highestcellx; 	        x location of highest cell in cluster
    int highestcelly;  		x location of highest cell in cluster
    float highesttowerenergy; 	 Energy of highest energy tower
  float highestcellenergy;    Energy of highest energy cell
    int highestcellz; 	   x location of highest cell in cluster
    float highestzE; 	   Energy of highest energy z slice
    int firstz;		   z index of first z hit
    int lastz;		   z index of last z hit
    int sizez;		   number of z layers with hits in them
    int hitTowers;	   average number of XY hits per z layer
    float energy;	   overall energy of cell
    float eventenergy;	   energy of event clustering was called on.



An analysis which calls these is ShapeAnalyzer.cc, which outputs many different plots informed by the clutering

It is run on the exact same files as EventDisplay (see github) with the same settings (adc, full, etc).

Can be run as follows:


bash runShapeAnalyzer_TBPST10_2026.sh $USR ${adc,full} $NEvt

which run is run on is manually set within the runShapeAnalyzer file for now
