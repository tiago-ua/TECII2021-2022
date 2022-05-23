void Histograma_D(){

TFile *ficheiro=new TFile("AmberTarget_Run_0.root","READ");
TFile *ficheiroGravar=new TFile("Analise.root","RECREATE");
TTree *dados=(TTree*)ficheiro->Get("edep_Per_Event");

Int_t nBins=500;
Double_t minBin=0.0;
Double_t maxBin=400000;

Int_t nHistos=4;
TH1D* histoDetetor[nHistos];
TCanvas *canvas[nHistos];
TString branchName;
TString canvasName;

for (Int_t i=0;i<nHistos; i++){
	TString histoName="histoDetetor"+TString::Itoa(i,10);
	histoDetetor[i]=new TH1D(histoName, histoName, nBins, minBin, maxBin);
	branchName="detector"+TString::Itoa(i,10);
	canvasName="canvas"+TString::Itoa(i,10);
	canvas[i]=new TCanvas(canvasName, canvasName);
	dados->Draw(branchName+">>"+histoName,branchName+">0","Same");
	histoDetetor[i]->SetTitle(histoName);
	histoDetetor[i]->Write();
	
	}

}
