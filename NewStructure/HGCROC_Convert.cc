#include "HGCROC_Convert.h"

#include "Analyses.h"
#include "Event.h"
#include "Tile.h"
#include "HGCROC.h"
#include "CommonHelperFunctions.h"
#include "Setup.h"

#include "include/h2g_decode/hgc_decoder.h"


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
    auto decoder = new hgc_decoder((char*)analysis->ASCIIinputName.Data(), 1, 4);
    for (auto ae : *decoder) {
        if (true || event_number % 100 == 0) {
            // std::cout << "\rFitting event " << event_number << std::flush;
        }
        // aligned_event *ae = *it;
        analysis->event.SetEventID(event_number);
        event_number++;
        // std::cout << "\nEvent: " << event_number << std::endl;
        // Loop over each tile
        for (int i = 0; i < ae->get_num_fpga(); i++) {
            // std::cout << "\nFPGA: " << i << std::endl;
            auto single_kcu = ae->get_event(i);
            // std::cout << "Number of samples: " << single_kcu->get_n_samples() << std::endl;
            for (int j = 0; j < ae->get_channels_per_fpga(); j++) {
                // std::cout << "\nChannel: " << j << std::endl;
                int channel_number = i * ae->get_channels_per_fpga() + j;
                // std::cout << "Channel number: " << channel_number << std::endl;
                int x, y, z;
                if (decode_position(channel_number, x, y, z)) {
                    Hgcroc *tile = new Hgcroc();
                    auto cell_id = analysis->setup->GetCellID(y, x, z, 0);  // needs to be adapted once we have multiple modules
                    tile->SetCellID(cell_id);        // TODO: This is not the same cell ID as Fredi and Vincent set up
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
                        tile->AppendWaveformADC(single_kcu->get_sample_adc(j, sample));
                        tile->AppendWaveformTOA(single_kcu->get_sample_toa(j, sample));
                        tile->AppendWaveformTOT(single_kcu->get_sample_tot(j, sample));
                    }

                    // process tile waveform
                    waveform_builder->set_waveform(tile->GetADCWaveform());
                    waveform_builder->fit();
                    tile->SetE(waveform_builder->get_E());
                    tile->SetPedestal(waveform_builder->get_pedestal());

                    analysis->event.AddTile(tile);
                }
            }
        // Fill the event
        analysis->TdataOut->Fill();
        analysis->event.ClearTiles();
        }
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

bool decode_position(int channel, int &x, int &y, int &z) {
    int channel_map[72] = {64, 63, 66, 65, 69, 70, 67, 68,  // this goes from 0 to 63 in lhfcal space to 0 to 71 in asic space
                           55, 56, 57, 58, 62, 61, 60, 59,  // So channel 0 on the detector is channel 64 on the asic
                           45, 46, 47, 48, 52, 51, 50, 49,  // What we want is the reverse of this, going from 0 to 71 in asic
                           37, 36, 39, 38, 42, 43, 40, 41,  // space to 0 to 63 in lhfcal space
                           34, 33, 32, 31, 27, 28, 29, 30,
                           25, 26, 23, 24, 20, 19, 22, 21,
                           16, 14, 15, 12,  9, 11, 10, 13,
                            7,  6,  5,  4,  0,  1,  2,  3,
                            -1, -1, -1, -1, -1, -1, -1, -1};

    int fpga_factor[4] = {1, 3, 0, 2};

    int asic_channel = channel % 72;
    // find the lhfcal channel
    int lhfcal_channel = -1;
    for (int i = 0; i < 72; i++) {
        if (channel_map[i] == asic_channel) {
            lhfcal_channel = i;
            break;
        }
    }

    if (lhfcal_channel == -1) {
        // These are the empty channels
        x = -1;
        y = -1;
        z = -1;
        return false;
    }

    int candy_bar_index = lhfcal_channel % 8;
    if (candy_bar_index < 4) {
        y = 1;
    } else {
        y = 0;
    }

    if (candy_bar_index == 0 || candy_bar_index == 7) {
        x = 0;
    } else if (candy_bar_index == 1 || candy_bar_index == 6) {
        x = 1;
    } else if (candy_bar_index == 2 || candy_bar_index == 5) {
        x = 2;
    } else {
        x = 3;
    }

    int fpga = channel / 144;
    int asic = (channel % 144) / 72;

    z = fpga_factor[fpga] * 16 + asic * 8 + lhfcal_channel / 8;

    return true;
}