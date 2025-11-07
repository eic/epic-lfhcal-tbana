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
                                    analysis->setup->GetNMaxKCUs(),           // number of kcu's to be aligned & read out
                                    5,                                        // debug level
                                    analysis->GetHGCROCTrunctation());         // switch to enable artificial truncation to 8 bit, disregarding 2 least significant bits
    for (auto ae : *decoder) {
        if (true || event_number % 100 == 0) {
            std::cout << "\rFitting event " << event_number << std::flush;
        }
        if (analysis->maxEvents != -1 && event_number > analysis->maxEvents ){
          break;  
        }
        // aligned_event *ae = *it;
        // aligned_event *ae = *it;
        analysis->event.SetEventID(event_number);
        event_number++;
        // std::cout << "\nEvent: " << event_number << std::endl;
        // std::cout << "\nEvent: " << event_number << std::endl;
        // Loop over each tile
        for (int i = 0; i < ae->get_num_fpga(); i++) {
            // std::cout << "\nFPGA: " << i << std::endl;
            // std::cout << "\nFPGA: " << i << std::endl;
            auto single_kcu = ae->get_event(i);
            // std::cout << "Number of samples: " << single_kcu->get_n_samples() << std::endl;
            // std::cout << "Number of samples: " << single_kcu->get_n_samples() << std::endl;
            for (int j = 0; j < ae->get_channels_per_fpga(); j++) {
                // std::cout << "\nChannel: " << j << std::endl;
                // std::cout << "\nChannel: " << j << std::endl;
                int channel_number = i * ae->get_channels_per_fpga() + j;
                // std::cout << "Channel number: " << channel_number << std::endl;
                int asic = i * 2 + (j / 72);
                
                auto cell_id = analysis->setup->GetCellID(asic, j % 72);
                if (analysis->debug > 0 && event_number == 1) {
                    std::cout << Form("KCU: %d, asic: %d , channel %d,  %s", i, int(j / 72),  j % 72, (analysis->setup->DecodeCellID(cell_id)).Data()) << std::endl;
                }
                
                if (cell_id != -1) {
                // std::cout << "Channel number: " << channel_number << std::endl;
                int asic = i * 2 + (j / 72);
                
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
