//
// Created by ilker on 6/27/22.
//

#include <G4SIunits.hh>
#include "FileHandling.h"
#include "G4Exception.hh"
namespace nexus{
    using namespace CLHEP;
    // construct
    FileHandling::FileHandling() {

    }
    //Destruct
    FileHandling::~FileHandling() {
        if (fstfile.is_open()) fstfile.close();
        if(ifsfile.is_open()) ifsfile.close();
    }

    // This function imports data with two column with any delimeter
    vector<vector<G4double>>  FileHandling::GetData(string file, char del,G4int SkipRow=1)
    {
        string str;
        ifsfile =ifstream (file);

        if (!ifsfile.is_open()) {
            G4Exception("FileHandling","[Get2ColumnData]",FatalException,"Could not open the file!");
        }
        vector<vector<G4double>> Data2d;
        vector<G4double>c1,c2;

        G4int SkipCount=0;

        while(getline(ifsfile,str)){
            string val;
            stringstream sline(str);
            if(SkipRow!=0 and SkipCount<SkipRow) {
                G4cout<<"Skipping following lines "<<G4endl;
                G4cout<<str<< " is Skipped "<<G4endl;
                SkipCount++;
            } else{
                int Counter=0;
                while (getline(sline,val,del)){
                    if(Counter==0)
                        c1.push_back(stof(val));
                    else
                        c2.push_back(stof(val));
                    Counter++;
                }

            }

        }
        Data2d.push_back(c1);
        Data2d.push_back(c2);
        return Data2d;

    }
    void FileHandling::SaveToTextFile(string file, string labels, char del , std::vector<vector<G4double>>data) {
        string val;
        fstfile = fstream (file);
        if (!fstfile.is_open()) G4Exception("FileHandling","[SaveToTextFile]",FatalException,"Couldnt open the file!");


        stringstream sline(labels);
        G4int counter=0;

        // Checking to see if something written in the file, if it is skip labels
        getline(fstfile,val);
        if(!labels.empty()){
            if(val.size()==0)
                fstfile << labels;
        }

        if (data.size()==0) G4Exception("FileHandling","[SaveToTextFile]",FatalException,"Data array is empty!");

        for (int i=0; i<data.size();i++){
            string str;

            for (int k=0;k<data.at(i).size();k++){
                if(k==0)
                    str=to_string(data.at(i).at(k));
                else if(k==data.at(i).size()-1)
                    str=str+del+to_string(data.at(i).at(k)) +"\n";
                else
                    str=str+del+ to_string(data.at(i).at(k));
            }
            fstfile << str;
        }

        fstfile.close();
    }

    void FileHandling::SaveToTextFile(string file, string labels, G4String data) {
        string val;
        if(!fstfile.is_open()) fstfile=fstream (file.c_str(),std::fstream::in | std::fstream::out | std::fstream::app);

        if (!fstfile.is_open()) G4Exception("FileHandling","[SaveToTextFile]",FatalException,"Couldnt open the file!");


        stringstream sline(labels);
        G4int counter=0;

        // Checking to see if something written in the file, if it is skip labels
        getline(fstfile,val);

        if(!labels.empty()){
            if(val.empty()) fstfile << labels;
        }


        if (data.size()==0) G4Exception("FileHandling","[SaveToTextFile]",FatalException,"Data array is empty!");
        fstfile << data <<"\n";

        G4cout<<data << " is written to file "<<G4endl;
        fstfile.close();
    }


} // Namespace nexus is closed