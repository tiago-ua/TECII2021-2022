
// Gamma_Camera.C
// Macro for the evaluation of the Gamma_Camera EduGATE example.
// The user is allowed to choose a specific .root file for evaluation
// via a file browser.


#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TCanvas.h>
#include <TRootEmbeddedCanvas.h>
#include <TF2.h>
#include <RQ_OBJECT.h>
#include <TGNumberEntry.h>




class MyMainFrame : public TGMainFrame {

private:

   TGMainFrame *fMain;
   TGTextButton     *choose, *exit;
	TGNumberEntry *minEnergyEntry, *maxEnergyEntry;
   TRootEmbeddedCanvas *fEcanvas;
  
	TH1F *primary;
	TH1F *sc_ph;
	TH1F *sc_cry;
	TH1F *sc_com;
	TH1F *sc_ta ;
	TH1F *sc_col;
	TH1F *ener;
	
	TH2F *globalPosZ_Y;
	TH2F *globalPosZ_Y_Ewin;

	Float_t LETH;
	Float_t UETH;
	Bool_t firstTime=true;
	
public:
   MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~MyMainFrame();

   // slots

   void ChooseFile();
   void SetMinEnergyWindow();
   void SetMaxEnergyWindow();
   // void SetMaxEnergyWindow();
   // void GammaCamera(TString fname);
   void GammaCamera();
//cdazevedo
   void CloseWindow();




   ClassDef(MyMainFrame, 0)



};



class FileList {
	
	RQ_OBJECT("FileList")
	
protected:
	TGTransientFrame *fMain;
	TGFileContainer  *fContents;
	TGPopupMenu      *fMenu;	
	
	void DisplayFile(const TString &fname);
	void DisplayDirectory(const TString &fname);
	void DisplayObject(const TString& fname,const TString& name);
	
public:
	//cdazevedo
	//FileList(const TGWindow *p, const TGWindow *main, TCanvas *can, UInt_t w, UInt_t h);
	FileList(const TGWindow *p, const TGWindow *mainFrame, UInt_t w, UInt_t h);
	virtual ~FileList();
	
	// slots
	void OnDoubleClick(TGLVEntry*,Int_t);
	void DoMenu(Int_t);
	void CloseWindow();
	void SetFileName(const TString &fname){rootFileName=fname;};
	TString GetFileName(){return rootFileName;};

	TString rootFileName;
	
};



//cdazevedo
MyMainFrame *mainFrame;
FileList *theFileList;



void MyMainFrame::CloseWindow()
{
	// Got close message for this MainFrame. Terminates the application.
	
	gApplication->Terminate(0);
}

void MyMainFrame::ChooseFile()
{
  // Slot connected to the Clicked() signal. 
  // It will open a File Browser which allows to choose a file for processing.
  
  choose->SetState(kButtonDown);
  choose->SetText("&Select Root File");  
  theFileList=new FileList(gClient->GetRoot(), fMain,  500, 500);
  choose->SetState(kButtonUp);
}


void MyMainFrame::SetMinEnergyWindow(){

	LETH=minEnergyEntry->GetNumberEntry()->GetNumber();
	cout<<"minEnergyWindow changed to: "<<LETH<<endl;
	mainFrame->GammaCamera();

}

void MyMainFrame::SetMaxEnergyWindow(){

	UETH=maxEnergyEntry->GetNumberEntry()->GetNumber();
	cout<<"maxEnergyWindow changed to: "<<UETH<<endl;
	mainFrame->GammaCamera();

}


MyMainFrame::MyMainFrame(const TGWindow *p, UInt_t w, UInt_t h)
{

	// Create a main frame
	fMain = new TGMainFrame(p,w,h);
	fMain->Connect("CloseWindow()", "MyMainFrame", this, "CloseWindow()");
	fMain->DontCallClose(); // to avoid double deletions.
	
	// Create canvas widget
	fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,1050, 750); 
	fEcanvas->GetCanvas()->Divide(2,2);
	fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1));
	// Create a horizontal frame widget with buttons
	TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40); 
	choose = new TGTextButton(hframe,"&Select Root File"); 
	choose->Connect("Clicked()","MyMainFrame",this,"ChooseFile()"); 
	hframe->AddFrame(choose, new TGLayoutHints(kLHintsCenterX,5,5,3,4)); 


// TGNumberEntry (const TGWindow *parent=nullptr, Double_t val=0, Int_t digitwidth=5, Int_t id=-1, EStyle style=kNESReal, EAttribute attr=kNEAAnyNumber, ELimit limits=kNELNoLimits, Double_t min=0, Double_t max=1)
	minEnergyEntry= new TGNumberEntry(hframe,0,3,-1,TGNumberFormat::kNESRealTwo,TGNumberFormat::kNEAPositive,TGNumberFormat::kNELNoLimits,0,1);
	maxEnergyEntry= new TGNumberEntry(hframe,0,3,-1,TGNumberFormat::kNESRealTwo,TGNumberFormat::kNEAPositive,TGNumberFormat::kNELNoLimits,0,1);
	minEnergyEntry->Connect("ValueSet(Long_t)","MyMainFrame",this,"SetMinEnergyWindow()");//button action
	// (minEnergyEntry->GetNumberEntry())->Connect("ReturnPressed()", "MyMainFrame", this,"SetMinEnergyWindow()"); //number entered
	maxEnergyEntry->Connect("ValueSet(Long_t)","MyMainFrame",this,"SetMaxEnergyWindow()");//button action
	// (maxEnergyEntry->GetNumberEntry())->Connect("ReturnPressed()", "MyMainFrame", this,"SetMaxEnergyWindow()");//number entered





	// TGLayoutHints (ULong_t hints=kLHintsNormal, Int_t padleft=0, Int_t padright=0, Int_t padtop=0, Int_t padbottom=0)
	hframe->AddFrame(minEnergyEntry,new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	hframe->AddFrame(maxEnergyEntry,new TGLayoutHints(kLHintsCenterX,5,5,3,4));

	exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)"); 
	hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
	fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
	// Set a name to the main frame
	fMain->SetWindowName("Gamma Camera");
	// Map all subwindows of main frame
	fMain->MapSubwindows();
	// Initialize the layout algorithm
	fMain->Resize(fMain->GetDefaultSize());
	// Map main frame
	fMain->MapWindow();
	
	
	primary = new TH1F("primary","",100,0,1);
	sc_ph   = new TH1F("sc_ph","",100,0,1);
	sc_cry  = new TH1F("sc_mo","",100,0,1);
	sc_com  = new TH1F("sc_com","",100,0,1);
	sc_ta   = new TH1F("sc_ta","",100,0,1);
	sc_col  = new TH1F("sc_col","",100,0,1);
	ener    = new TH1F("ener","",100,0,1);

	//cdazevedo
	globalPosZ_Y = new TH2F("globalPosZ","globalPosY ",600,-150.0,+150.0,400,-100.0,+100.0);
	globalPosZ_Y_Ewin = new TH2F("globalPosZ_Ewin","globalPosY_Ewin",600,-150.0,+150.0,400,-100.0,+100.0);
}

void MyMainFrame::GammaCamera()
{
	
TString fname=theFileList->GetFileName();
 // cout<<"\nAQUI: "<<theFileList->GetFileName()<<" END"<<endl;


	TString RootFileName = fname;
	TFile f(RootFileName);
	
	TString GraphicFileName = RootFileName.Remove(RootFileName.Length()-5) + ".png";	

	TH1 *total_nb_primaries = (TH1*)gDirectory->Get("total_nb_primaries");
	TTree *Singles = (TTree*)gDirectory->Get("Singles");
	//cdazevedo
	TTree *Hits = (TTree*)gDirectory->Get("Hits");
	
	Int_t           comptonPhantom;
	Int_t           comptonCrystal;
	Int_t           RayleighCrystal;
	Int_t           RayleighPhantom;
	Float_t         energy;
	Char_t          comptVolName[40];
	Float_t			scatter_phantom, scatter_null, scatter_compartment, scatter_table;
	Float_t			scatter_collim, scatter_shielding, primary_event, scatter_crystal;
	Float_t	        globalPosX, globalPosY, globalPosZ, sourcePosX, sourcePosY, sourcePosZ;
	Double_t        time;	
	
	Int_t			runID, runID_hits;
	Int_t			eventID, eventID_hits;
	Char_t          processName[40];
	Int_t			PDGEncoding;	
	
	Singles->SetBranchAddress("comptonPhantom",&comptonPhantom);
	Singles->SetBranchAddress("comptonCrystal",&comptonCrystal);
	Singles->SetBranchAddress("RayleighCrystal",&RayleighCrystal);
	Singles->SetBranchAddress("RayleighPhantom",&RayleighPhantom);
	Singles->SetBranchAddress("energy",&energy);
	Singles->SetBranchAddress("comptVolName",comptVolName);
	
	Singles->SetBranchAddress("globalPosX",&globalPosX);
	Singles->SetBranchAddress("globalPosY",&globalPosY);	
	Singles->SetBranchAddress("globalPosZ",&globalPosZ);
	
	Singles->SetBranchAddress("sourcePosX",&sourcePosX);
	Singles->SetBranchAddress("sourcePosY",&sourcePosY);
	Singles->SetBranchAddress("sourcePosZ",&sourcePosZ);
	Singles->SetBranchAddress("time",&time);
	
	Singles->SetBranchAddress("runID", &runID);
	Singles->SetBranchAddress("eventID", &eventID);
	
	Hits->SetBranchAddress("processName",processName);
	Hits->SetBranchAddress("PDGEncoding",&PDGEncoding);
	Hits->SetBranchAddress("runID", &runID_hits);
	Hits->SetBranchAddress("eventID", &eventID_hits);	
	
	//*******************************************************
	Singles->Draw("energy>>hi_energy","","goff");
	TH1F *hi_energy = (TH1F*)gDirectory->Get("hi_energy");
	Int_t   nbins     = hi_energy->GetNbinsX();
	Float_t binWidth  = hi_energy->GetBinWidth(1);
	Float_t	Emax      = nbins*binWidth + 0.05;
	//*******************************************************
	
	//	
	// Reset Histograms
	//
	primary->Reset();
	primary->SetBins(100,0,Emax);
	sc_ph->Reset();
	sc_ph->SetBins(100,0,Emax);
	sc_cry->Reset();
	sc_cry->SetBins(100,0,Emax);
	sc_com->Reset();
	sc_com->SetBins(100,0,Emax);
	sc_ta->Reset();
	sc_ta->SetBins(100,0,Emax);
	sc_col->Reset();
	sc_col->SetBins(100,0,Emax);
	ener->Reset();
	ener->SetBins(100,0,Emax);
	
	globalPosZ_Y->Reset();
	globalPosZ_Y_Ewin->Reset();


if (firstTime==true){

	LETH = Singles->GetMinimum("energy");
	UETH = Singles->GetMaximum("energy");
//update number entries
	minEnergyEntry->SetNumber(LETH);
	maxEnergyEntry->SetNumber(UETH);

firstTime=false;
}
	cout << endl;
	cout << " Energy setting (leth, ueth):  " << LETH << " " <<  UETH << endl;
	cout << endl;
	cout << "##### Reading from file: " << RootFileName << endl;
	cout << endl;
	
	Int_t nentries = Singles->GetEntries();
	Int_t ntotal = total_nb_primaries->GetMean();
	Int_t nbytes = 0;
	
	//	cout<<"##### nb of entries: " << nentries <<endl;
	
	for (Int_t i=0; i<nentries;i++) {
		//for (Int_t i=0; i<10;i++) {	
		nbytes += Singles->GetEntry(i);
		
		globalPosZ_Y->Fill(globalPosZ,globalPosY);
		
		if (energy >= LETH && energy <= UETH) {
			globalPosZ_Y_Ewin->Fill(globalPosZ,globalPosY);
		}
		
		ener->Fill(energy);
		
		if (comptonPhantom == 0 && comptonCrystal == 0) { 
			primary->Fill(energy);
			primary_event++;
			//globalPosZ_Y_Ewin->Fill(globalPosZ,globalPosY);
		} // primary
		
		if (comptonCrystal != 0 && comptonPhantom == 0) {
			scatter_crystal++;
			sc_cry->Fill(energy);
		} // scatter in crystal
		
		// if (strcmp(comptVolName,"Phantom_phys")==  NULL||strcmp(comptVolName,"source_vol_phys") == NULL) {
		if (strcmp(comptVolName,"Phantom_phys")==  0||strcmp(comptVolName,"source_vol_phys") == 0) {
			scatter_phantom++;
			sc_ph->Fill(energy);
		}	// scatter in phantom
		
		
		if (strcmp(comptVolName,"compartment_phys") == 0) {
			scatter_compartment++;
			sc_com->Fill(energy);
		}
		
		if (strcmp(comptVolName,"table_phys") == 0) {			
			scatter_table++;
			sc_ta->Fill(energy);
		}
		
		if (strcmp(comptVolName,"collimator_phys") == 0) {
			scatter_collim++;
			sc_col->Fill(energy);
		}
		
		
		
	} // nentries
	
	
	// **************************************** Plots **********************************************
	

	


	gStyle->SetPalette(1);
	gStyle->SetOptStat(1111);
	gStyle->SetOptFit(1111);
	// First Canvas
	
	TCanvas *C1=fEcanvas->GetCanvas();
	
	C1->SetFillColor(0);
	C1->cd(1);
	
	ener->SetFillColor(2);
	ener->GetXaxis()->SetTitle("MeV");
	ener->GetYaxis()->SetTitle("# entries");
	ener->Draw();
	TLatex *   tex = new TLatex(0.0169022,11741.43,"Total Spectrum of the detected events");
	tex->SetLineWidth(2);
	tex->Draw();
	
	
	 // TLine *lineMin=new TLine(LETH, ener->GetMinimum(), LETH, ener->GetMaximum());
	 TBox *theEnergyBox=new TBox(LETH, ener->GetMinimum(), UETH, ener->GetMaximum());
   theEnergyBox->SetFillStyle(3002);
	theEnergyBox->SetFillColorAlpha(kGreen, 0.25);
	theEnergyBox->Draw();

	TLegend *theLegend = new TLegend(0.1,0.75,0.4,0.9);
	theLegend->SetFillColor(0);
	theLegend->SetTextSize(0.03);
	theLegend->AddEntry(ener,"Energy histogram","f");
	theLegend->AddEntry(theEnergyBox,"Energy window selected","f");
	theLegend->Draw();





	C1->SetFillColor(0);
	C1->cd(2);
	
	primary->Draw();
	primary->SetLineColor(6);
	primary->GetXaxis()->SetTitle("MeV");
	primary->GetYaxis()->SetTitle("# entries");
	sc_ph->Draw("same");
	sc_ph->SetLineColor(1);
	
	sc_ta->Draw("same");
	sc_ta ->SetLineColor(2);
	
	sc_col->Draw("same");
	sc_col ->SetLineColor(3);
	
	sc_com->Draw("same");
	sc_com ->SetLineColor(5);
	
	sc_cry->Draw("same");
	sc_cry ->SetLineColor(4);
	
	TLegend *leg1 = new TLegend(0.1,0.6,0.5,0.9);
	leg1->SetFillColor(0);
	leg1->SetTextSize(0.03);
	leg1->AddEntry(primary,"primary spectrum                 ","l");
	leg1->AddEntry(sc_ph,  "scatter in the phantom           ","l");
	leg1->AddEntry(sc_ta,  "scatter in the table             ","l");
	leg1->AddEntry(sc_col, "scatter in the collimator        ","l");
	leg1->AddEntry(sc_com, "scatter in the backcompartment   ","l");
	leg1->AddEntry(sc_cry, "scatter in the crystal           ","l");
	leg1->Draw();
	
	tex = new TLatex(-0.0040358,10141.43,"Primary and scatter spectra of the detected events");
	tex->SetLineWidth(2);
	tex->Draw();
	
	C1->SetFillColor(0);
	C1->cd(3);
	
	
	globalPosZ_Y_Ewin->GetXaxis()->SetTitle("globalPosZ (mm)");
	globalPosZ_Y_Ewin->GetYaxis()->SetTitle("globalPosY (mm)");
	globalPosZ_Y_Ewin->Draw("COLZ");

	
	TCanvas *dummyCanvas=new TCanvas();
	dummyCanvas->cd();
	// dummyCanvas->SetGrayscale();
	globalPosZ_Y_Ewin->GetXaxis()->SetTitle("globalPosZ (mm)");
	globalPosZ_Y_Ewin->GetYaxis()->SetTitle("globalPosY (mm)");
	globalPosZ_Y_Ewin->Draw("COLZ");
	dummyCanvas->SaveAs(GraphicFileName);
	delete dummyCanvas;
	

	C1->cd(4);

	TH1D* projection_X_Ewin=globalPosZ_Y_Ewin->ProjectionX("Projection_X", 0,-1,"");
	
	projection_X_Ewin->GetXaxis()->SetTitle("globalPosZ (mm)");
	projection_X_Ewin->GetYaxis()->SetTitle("Counts");
	projection_X_Ewin->SetTitle("Projection");
	
	projection_X_Ewin->Fit("gaus");
	projection_X_Ewin->DrawCopy();

	C1->SetFillColor(0);

	// save Plot to disk 
	
	C1->Update();    
	
	
	// **************************************** Summary Printout **********************************************
	
	cout << " *********************************************************************************** " << endl;
	cout << " *                                                                                 * " << endl;
	cout << " *   E d u G a t e  S i m u l a t i o n   A n a l y s i s                          * " << endl;
	cout << " *   G A M M A  C A M E R A                                                        * " << endl;
	cout << " *                                                                                 * " << endl;
	cout << " *********************************************************************************** " << endl;
	cout << endl;
	
	cout<<"##### Number of emitted particles    :  "	<<ntotal<<endl;
	cout<<"##### Number of detected events      :  "	<<nentries<<endl;
	cout<<"##### ratio detected/emitted         :  "	<<float(nentries)/float(ntotal)*100<<" %"<<endl;
	cout<<"##### Primary events                 :  "	<<primary_event/nentries*100       <<" %"<<endl;
	cout<<"##### Scatter in the phantom         :  "	<<scatter_phantom/nentries*100     <<" %"<<endl;
	cout<<"##### Scatter in the table           :  "	<<scatter_table/nentries*100       <<" %"<<endl;
	cout<<"##### Scatter in the collimator      :  "	<<scatter_collim/nentries*100      <<" %"<<endl;
	cout<<"##### Scatter in the crystal         :  "	<<scatter_crystal/nentries*100     <<" %"<<endl;
	cout<<"##### Scatter in the backcompartment :  "	<<scatter_compartment/nentries*100 <<" %"<<endl;
	cout << endl;
	cout << endl;
	
}	

MyMainFrame::~MyMainFrame()
{
   // Clean up all widgets, frames and layouthints that were used
   fMain->Cleanup();
	delete fMain;
	delete primary;
	delete sc_ph;
	delete sc_cry;
	delete sc_com;
	delete sc_ta ;
	delete sc_col;
	delete ener;
	delete globalPosZ_Y;
	delete globalPosZ_Y_Ewin;
	
}

FileList::FileList(const TGWindow *p, const TGWindow *mainFrame, UInt_t w, UInt_t h)
{
   // Create transient frame containing a filelist widget.

   TGLayoutHints *lo;	
//cdazevedo
 //  fMain = new TGTransientFrame(p, main, w, h);
 fMain = new TGTransientFrame(p, mainFrame, w, h);
   fMain->Connect("CloseWindow()", "FileList", this, "CloseWindow()");
   fMain->DontCallClose(); // to avoid double deletions.

   // use hierarchical cleaning
   fMain->SetCleanup(kDeepCleanup);

   TGMenuBar* mb = new TGMenuBar(fMain);
   lo = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
   fMain->AddFrame(mb, lo);

   fMenu = mb->AddPopup("&View");
   fMenu->AddEntry("Lar&ge Icons",kLVLargeIcons);
   fMenu->AddEntry("S&mall Icons",kLVSmallIcons);
   fMenu->AddEntry("&List",       kLVList);
   fMenu->AddEntry("&Details",    kLVDetails);
   fMenu->AddSeparator();
   fMenu->AddEntry("&Close",      10);
   fMenu->Connect("Activated(Int_t)","FileList",this,"DoMenu(Int_t)");

   TGListView* lv = new TGListView(fMain, w, h);
   lo = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   fMain->AddFrame(lv,lo);

   Pixel_t white;
   gClient->GetColorByName("white", white);
   fContents = new TGFileContainer(lv, kSunkenFrame,white);
   fContents->Connect("DoubleClicked(TGFrame*,Int_t)", "FileList", this,
                      "OnDoubleClick(TGLVEntry*,Int_t)");

   // position relative to the parent's window
   fMain->CenterOnParent();

   fMain->SetWindowName("Choose Root File");
   fMain->MapSubwindows();
   fMain->MapWindow();
   fContents->SetDefaultHeaders();
   fContents->DisplayDirectory();
   fContents->AddFile("..");        // up level directory
   fContents->Resize();
   fContents->StopRefreshTimer();   // stop refreshing
   fContents->SetViewMode(kLVDetails);
   fMain->Resize();
}

FileList::~FileList()
{
   // Cleanup.

   delete fContents;
   delete fMenu;
   fMain->DeleteWindow();  // deletes fMain
}

void FileList::DoMenu(Int_t mode)
{
   // Switch view mode.

   if (mode<10) {
      fContents->SetViewMode((EListViewMode)mode);
   } else {
      delete this;
   }
}

void FileList::DisplayFile(const TString &fname)
{
   // Display content of ROOT file.
	cout<<fname<<endl;
   
   theFileList->SetFileName(fname);

	// mainFrame->GammaCamera(fname);
	mainFrame->GammaCamera();
	fMain->CloseWindow();

}

void FileList::DisplayDirectory(const TString &fname)
{
   // Display content of directory.

   fContents->SetDefaultHeaders();
   gSystem->ChangeDirectory(fname);
   fContents->ChangeDirectory(fname);
   fContents->DisplayDirectory();
   fContents->AddFile("..");  // up level directory
   fMain->Resize();
}

void FileList::DisplayObject(const TString& fname,const TString& name)
{
   // Browse object located in file.

   TDirectory *sav = gDirectory;

   static TFile *file = 0;
   if (file) delete file;     // close

   file = new TFile(fname);   // reopen

   TObject* obj = file->Get(name);
   if (obj) {
      if (!obj->IsFolder()) {
         obj->Browse(0);
      } else obj->Print();
   }
   gDirectory = sav;
}

void FileList::OnDoubleClick(TGLVEntry *f, Int_t btn)
{
   // Handle double click.

   if (btn != kButton1) return;

   TString name(f->GetTitle());
   const char* fname = (const char*)f->GetUserData();

   if (fname) {
      DisplayObject(fname, name);
   } else if (name.EndsWith(".root")) {
      DisplayFile(name);
   } else {
      DisplayDirectory(name);
   }
	
}




void FileList::CloseWindow()
{
   delete this;
}


void Gamma_Camera()
{
   // Popup the GUI...
//cdazevedo
   //main = new MyMainFrame(gClient->GetRoot(), 350, 80);
   mainFrame = new MyMainFrame(gClient->GetRoot(), 350, 80);
}
