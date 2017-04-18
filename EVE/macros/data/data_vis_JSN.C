/// \file data_visAD0.C
/// \brief Data visualisation macro for AD
///
/// This macro is a starting point for visualisation of AD data. Available data types: Raw, Hits, ESD.
///
/// Parameters which can be modified in AliEVE config file:
/// AD.maxCharge.Raw - maximum expected charge in Raw data, used for colouring
/// AD.maxCharge.ESD - maximum expected charge in ESD, used for colouring
/// AD.showLegend - switch on/off legend's drawing
///
///
/// \author Matevz Tadel <Matevz.Tadel@cern.ch>, Univ. of California San Diego
/// \author Alja Mrak-Tadel <Alja.Mrak.Tadel@cern.ch>, Univ. of California San Diego
/// \author Michal Broz <Michal.Broz@cern.ch>, Czech Technical University
/// \author Jeremi Niedziela <jeremi.niedziela@cern.ch>, Warsaw University of Technology
/// \date Feb 23, 2016


/// Main function of the macro. Call data-specific function according to requested data type.

void data_vis_JSN(AliEveEventManager::EDataType)
{
    TEnv settings;
    AliEveInit::GetConfig(&settings);
    
    Color_t colors[15];
    // default color scheme by type:
    colors[0] = settings.GetValue("tracks.byType.electron",600);
    colors[1] = settings.GetValue("tracks.byType.muon",416);
    colors[2] = settings.GetValue("tracks.byType.pion",632);
    colors[3] = settings.GetValue("tracks.byType.kaon",400);
    colors[4] = settings.GetValue("tracks.byType.proton",797);
    colors[5] = settings.GetValue("tracks.byType.deuteron",797);
    colors[6] = settings.GetValue("tracks.byType.triton",797);
    colors[7] = settings.GetValue("tracks.byType.he3",797);
    colors[8] = settings.GetValue("tracks.byType.alpha",403);
    colors[9] = settings.GetValue("tracks.byType.photon",0);
    colors[10]= settings.GetValue("tracks.byType.pi0",616);
    colors[11]= settings.GetValue("tracks.byType.neutron",900);
    colors[12]= settings.GetValue("tracks.byType.kaon0",801);
    colors[13]= settings.GetValue("tracks.byType.elecon",920);
    colors[14]= settings.GetValue("tracks.byType.unknown",920);
    
    TEveElementList* cont = new TEveElementList("ESD Tracks by PID");
    gEve->AddElement(cont);
    
    const Int_t   nCont = 15;
    const Float_t maxR  = settings.GetValue("tracks.byType.animate",false) ? 0 : 520;
    const Float_t magF  = 0.5;
    
    TEveTrackList *tl[nCont];
    Int_t          tc[nCont];
    Int_t          count = 0;
    
    tl[0] = new TEveTrackList("Electrons");
    tl[1] = new TEveTrackList("Muons");
    tl[2] = new TEveTrackList("Pions");
    tl[3] = new TEveTrackList("Kaons");
    tl[4] = new TEveTrackList("Protons");
    tl[5] = new TEveTrackList("Deuterons");
    tl[6] = new TEveTrackList("Tritons");
    tl[7] = new TEveTrackList("He3");
    tl[8] = new TEveTrackList("Alpha");
    tl[9] = new TEveTrackList("Photons");
    tl[10]= new TEveTrackList("Pi0");
    tl[11]= new TEveTrackList("Neutrons");
    tl[12]= new TEveTrackList("Kaon0");
    tl[13]= new TEveTrackList("EleCon");
    tl[14]= new TEveTrackList("Unknown");
    
    for (int i=0; i<15; i++) {
        tc[i] = 0;
        
        tl[i]->GetPropagator()->SetMagField(magF);
        tl[i]->GetPropagator()->SetMaxR(maxR);
        tl[i]->SetMainColor(colors[i]);
        tl[i]->SetLineWidth(settings.GetValue("tracks.width",2));
        cont->AddElement(tl[i]);
    }
    
    int pid = -1;
    AliESDtrack* at = NULL;
    
    int numberOfTracks = 1;
    
    for (Int_t n = 0; n < numberOfTracks; ++n)
    {
        double v[3] = {0.0,0.0,0.0};
        double p[3] = {1.0,0.0,0.0};
        
        AliMinimalisticTrack *track = new AliMinimalisticTrack(1,10.,n,211,0.5,0.0,v,v,p,211,0.0,0.0,0.0,0);
        
        if(abs(track->GetPID())==211) pid = 0;
        
        
        TEveTrackList* tlist = tl[pid];
        ++tc[pid];
        ++count;
        
        AliEveTrack* eveTrack = MakeTrack(track, tlist);
        eveTrack->SetName(Form("ESD Track idx=%d, pid=%d", track->GetID(), pid));
        tlist->AddElement(eveTrack);
    }
    for (Int_t ti = 0; ti < nCont; ++ti)
    {
        TEveTrackList* tlist = tl[ti];
        tlist->SetName(Form("%s [%d]", tlist->GetName(), tlist->NumChildren()));
        tlist->SetTitle(Form("N tracks=%d", tc[ti]));
        tlist->MakeTracks();
    }
    cont->SetTitle(Form("N all tracks = %d", count));
    cont->FindListTreeItem(gEve->GetListTree())->SetOpen(kTRUE);
    
    gEve->Redraw3D();
}

AliEveTrack* MakeTrack(AliMinimalisticTrack *t, TEveTrackList* cont)
{
    AliEveTrack* track = new AliEveTrack(t->GetVertex(), t->GetMomentum(), t->GetBeta(), t->GetCharge(), t->GetID(), t->GetID(), cont->GetPropagator());
    
    track->SetAttLineAttMarker(cont);
    track->SetName(Form("AliEveTrack %d", t->GetID()));
    track->SetElementTitle(Form("AliEveTrack %d", t->GetID()));
//    track->SetSourceObject(at);

    return track;
}

