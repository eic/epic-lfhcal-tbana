#include "HGCROC_Convert.h"

#include "Analyses.h"
#include "Event.h"
#include "Tile.h"
#include "HGCROC.h"
#include "CommonHelperFunctions.h"
#include "Setup.h"
#include "TileSpectra.h"
#include "hgc_decoder.h"


int run_hgcroc_conversion(Analyses *analysis, waveform_fit_base *waveform_builder) {
    #ifdef DECODE_HGCROC // Built for HGCROC decoding
    std::cout << "Setting up event parameters for HGCROC data" << std::endl;
    // Make sure we actually have a waveform builder
    if (waveform_builder == nullptr) {
        std::cout << "No waveform builder specified" << std::endl;
        return 1;
    }

    // Check mapping file
    if (analysis->MapInputName.IsNull()) {
        std::cout << "No mapping file specified" << std::endl;
        // return 1;
    }
    analysis->setup->Initialize(analysis->MapInputName.Data(), analysis->debug);
    
    // Check run list file - Used to get run parameters automatically
    if (analysis->RunListInputName.IsNull()) {
        std::cout << "No run list file specified" << std::endl;
        // return 1;
    }
    std::map<int, RunInfo> ri = readRunInfosFromFile(analysis->RunListInputName.Data(), analysis->debug, 0);

    // Set up the file to convert
    if (analysis->ASCIIinputName.IsNull()) {
        std::cout << "No input file specified" << std::endl;
        // return 1;
    }

    
    std::map<int,TileSpectra> hSpectra;
    std::map<int, TileSpectra>::iterator ithSpectra;

    
    // Set up the static event parameters
    // Clean up file headers'
    // THIS WILL HAVE TO CHANGE
    TObjArray* tok = analysis->ASCIIinputName.Tokenize("/");
    // get file name
    TString file = ((TObjString*)tok->At(tok->GetEntries() - 1))->String();
    delete tok;
    tok=file.Tokenize("_");
    TString header = ((TObjString*)tok->At(0))->String();
    delete tok;
    
    // Get run number from file & find necessary run infos
    TString RunString=header(3,header.Sizeof());
    std::map<int,RunInfo>::iterator it=ri.find(RunString.Atoi());
    std::cout<<RunString.Atoi()<<"\t"<<it->second.species<<std::endl;
    analysis->event.SetRunNumber(RunString.Atoi());
    analysis->event.SetBeamName(it->second.species);
    analysis->event.SetBeamID(it->second.pdg);
    analysis->event.SetBeamEnergy(it->second.energy);
    analysis->event.SetVop(it->second.vop);
    analysis->event.SetVov(it->second.vop-it->second.vbr);
    analysis->event.SetBeamPosX(it->second.posX);
    analysis->event.SetBeamPosY(it->second.posY);
    analysis->calib.SetRunNumber(RunString.Atoi());
    analysis->calib.SetVop(it->second.vop);
    analysis->calib.SetVov(it->second.vop-it->second.vbr);  
    analysis->calib.SetBCCalib(false);
    analysis->event.SetROtype(ReadOut::Type::Hgcroc);

    // Run the event builder
    // std::list<aligned_event*> *events = new std::list<aligned_event*>();
    // for (auto event : *decoder) {
        // events->push_back(event);
        // }
        
        // std::cout << "\ncompleted HGCROC event builder!\n" << std::endl;
        // std::cout << "Number of events: " << events->size() << std::endl;
        
        
        // convert from the aligned_events datatype to the Event datatype
    int event_number = 0;
    if (analysis->maxEvents != -1){
        std::cout << "Event conversion will be stopped at " << analysis->maxEvents << std::endl;
    }
    
    auto decoder = new hgc_decoder( (char*)analysis->ASCIIinputName.Data(),   // filename for ascii file
                                    1,                                        // detector ID (1: LFHCal)
                                    it->second.nFPGA,                         // number of kcu's to be aligned & read out
                                    it->second.nASIC,                         // number of asic's per KCU
                                    3,                                        // debug level
                                    analysis->GetHGCROCTrunctation());        // switch to enable artificial truncation to 8 bit, disregarding 2 least significant bits
    for (auto ae : *decoder) {
        if (analysis->maxEvents != -1 && event_number > analysis->maxEvents ){
          break;  
        }
        // aligned_event *ae = *it;
        analysis->event.SetEventID(event_number);
        event_number++;
        if (event_number % 500 == 0){
          std::cout << "\nEvent: " << event_number << "\t"<< decoder->get_num_proc_events() << std::endl;
          for (int i = 0; i < (int)it->second.nFPGA; i++) {
              std::cout << "\t KCU: " << i << "\t att:" << decoder->get_attempted_waveforms(i) << "\t rec:" << decoder->get_completed_waveforms(i) << "\t in progress: " << decoder->get_inprogress_waveforms(i) << "\t aborted: " << decoder->get_discarded_waveforms(i) << std::endl;
          }
        }
        
        // Loop over each tile
        for (int i = 0; i < ae->get_num_fpga(); i++) {
            // std::cout << "\nFPGA: " << i << std::endl;
            auto single_kcu = ae->get_event(i);
            // std::cout << "Number of samples: " << single_kcu->get_n_samples() << std::endl;
            for (int j = 0; j < ae->get_channels_per_fpga(); j++) {
                // std::cout << "\nChannel: " << j << std::endl;
                int channel_number = i * ae->get_channels_per_fpga() + j;
                // std::cout << "Channel number: " << channel_number << std::endl;
                int asic = i * it->second.nASIC + (j / 72);
                
                auto cell_id = analysis->setup->GetCellID(asic, j % 72);
                if (analysis->debug > 0 && event_number == 1) {
                    std::cout << Form("KCU: %d, asic: %d , channel %d,  %s", i, int(j / 72),  j % 72, (analysis->setup->DecodeCellID(cell_id)).Data()) << std::endl;
                }
                
                if (cell_id != -1) {
                // std::cout << "Channel number: " << channel_number << std::endl;
                int asic = i * it->second.nASIC + (j / 72);
                
                auto cell_id = analysis->setup->GetCellID(asic, j % 72);
                if (analysis->debug > 0 && event_number == 1) {
                    std::cout << Form("KCU: %d, asic: %d , channel %d,  %s", i, int(j / 72),  j % 72, (analysis->setup->DecodeCellID(cell_id)).Data()) << std::endl;
                }
                
                if (cell_id != -1) {
                    Hgcroc *tile = new Hgcroc();
                    tile->SetCellID(cell_id);        
                    tile->SetCellID(cell_id);        
                    tile->SetROtype(ReadOut::Type::Hgcroc);
                    tile->SetLocalTriggerBit(0);            // What are these supposed to be?
                    tile->SetLocalTriggerPrimitive(0);
                    tile->SetE(0);                          // Need to process waveform to get this
                    tile->SetTOA(0);                        // Need to process waveform to get this
                    tile->SetTOT(0);                        // Need to process waveform to get this

                    tile->SetNsample(single_kcu->get_n_samples());
                    for (int sample = 0; sample < single_kcu->get_n_samples(); sample++) {
                        // std::cout << "Sample: " << sample;
                        // std::cout << " ADC: " << single_kcu->get_sample_adc(j, sample);
                        // std::cout << " TOA: " << single_kcu->get_sample_toa(j, sample);
                        // std::cout << " TOT: " << single_kcu->get_sample_tot(j, sample) << std::endl;
                        // std::cout << "Sample: " << sample;
                        // std::cout << " ADC: " << single_kcu->get_sample_adc(j, sample);
                        // std::cout << " TOA: " << single_kcu->get_sample_toa(j, sample);
                        // std::cout << " TOT: " << single_kcu->get_sample_tot(j, sample) << std::endl;
                        tile->AppendWaveformADC(single_kcu->get_sample_adc(j, sample));
                        tile->AppendWaveformTOA(single_kcu->get_sample_toa(j, sample));
                        tile->AppendWaveformTOT(single_kcu->get_sample_tot(j, sample));
                        if (single_kcu->get_sample_toa(j, sample) > 0) { // 
                            tile->SetTOA(single_kcu->get_sample_toa(j, sample));
                        }
                        if (single_kcu->get_sample_tot(j, sample) > 0) { // 
                            tile->SetTOT(single_kcu->get_sample_tot(j, sample));
                        }
                    }

                    // process tile waveform
                    waveform_builder->set_waveform(tile->GetADCWaveform());
                    waveform_builder->fit();
                    tile->SetIntegratedADC(waveform_builder->get_E());
                    if (tile->GetTOT() != 1)
                        tile->SetIntegratedTOT(tile->GetTOT());   // TODO: Placeholder
                    else 
                        tile->SetIntegratedTOT(0.);
                    if (waveform_builder->is_saturated()) {
                        tile->SetIntegratedValue(tile->GetIntegratedTOT()); // TODO: Placeholder
                    } else {
                        tile->SetIntegratedValue(tile->GetIntegratedADC()); // TODO: Placeholder
                    }
                    tile->SetIntegratedADC(waveform_builder->get_E());
                    if (tile->GetTOT() != 1)
                        tile->SetIntegratedTOT(tile->GetTOT());   // TODO: Placeholder
                    else 
                        tile->SetIntegratedTOT(0.);
                    if (waveform_builder->is_saturated()) {
                        tile->SetIntegratedValue(tile->GetIntegratedTOT()); // TODO: Placeholder
                    } else {
                        tile->SetIntegratedValue(tile->GetIntegratedADC()); // TODO: Placeholder
                    }
                    tile->SetPedestal(waveform_builder->get_pedestal());

                    analysis->event.AddTile(tile);
                }
            }
        // Fill the event
        }
        }
        analysis->TdataOut->Fill();
        analysis->event.ClearTiles();
    }
    
    std::cout << "\nFinished converting events\n" << std::endl;
    std::cout << "\nTotal Events: " << decoder->get_num_proc_events() << std::endl;
    for (int i = 0; i < (int)it->second.nFPGA; i++) {
        std::cout << "\t KCU: " << i << "\t att:" << decoder->get_attempted_waveforms(i) << "\t rec:" << decoder->get_completed_waveforms(i) << "\t in progress: " << decoder->get_inprogress_waveforms(i) << std::endl;
    }
    analysis->RootOutput->cd();
    
    // setup 
    RootSetupWrapper rswtmp=RootSetupWrapper(analysis->setup);
    analysis->rsw=rswtmp;
    analysis->TsetupOut->Fill();
    // calib
    analysis->TcalibOut->Fill();
    analysis->TcalibOut->Write();
    // event data
    analysis->TdataOut->Fill();
    analysis->TsetupOut->Write();
    analysis->TdataOut->Write();

    TH1D* hEventsKCU[it->second.nFPGA];
    long maxAttempted = 0;
    for (Int_t i = 0; i < (int)it->second.nFPGA; i++){
      hEventsKCU[i] = new TH1D( Form("hNEventsKCU%i", i),"event KCU category; events",4,-0.5,3.5);
      hEventsKCU[i]->SetBinContent(1, decoder->get_attempted_waveforms(i));
      hEventsKCU[i]->SetBinContent(2, decoder->get_completed_waveforms(i));
      hEventsKCU[i]->SetBinContent(3, decoder->get_discarded_waveforms(i));
      hEventsKCU[i]->SetBinContent(4, decoder->get_inprogress_waveforms(i));
      hEventsKCU[i]->Write();
      if (maxAttempted < decoder->get_attempted_waveforms(i))
        maxAttempted = decoder->get_attempted_waveforms(i);
    }
    
    TH1D* hEvents = new TH1D( "hNEvents","event category; events",2,-0.5,1.5);
    hEvents->SetBinContent(1, maxAttempted);
    hEvents->SetBinContent(2, decoder->get_num_proc_events());
    hEvents->Write();
    
    
    analysis->RootOutput->Close();
    return true;

    #else
    std::cout << "This code is not built for HGCROC decoding" << std::endl;
    analysis->RootOutput->cd();
    // setup 
    RootSetupWrapper rswtmp=RootSetupWrapper(analysis->setup);
    analysis->rsw=rswtmp;
    analysis->TsetupOut->Fill();
    // calib
    analysis->TcalibOut->Fill();
    analysis->TcalibOut->Write();
    // event data
    analysis->TdataOut->Fill();
    analysis->TsetupOut->Write();
    analysis->TdataOut->Write();

    analysis->RootOutput->Close();
    return false;
    
    #endif
}
