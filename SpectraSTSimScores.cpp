#include "SpectraSTSimScores.hpp"

#include <sstream>

/*

Program       : Spectrast
Author        : Henry Lam <hlam@systemsbiology.org>                                                       
Date          : 03.06.06 


Copyright (C) 2006 Henry Lam

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA

Henry Lam
Insitute for Systems Biology
1441 North 34th St. 
Seattle, WA  98103  USA
hlam@systemsbiology.org

*/

/* Class: SpectraSTSimScores
 * 
 * Manages the similarity scores. 
 * Needs to be modified for other scoring functions and for different output types.
 */

#define HEADERCELLCOLOR   "#42D4FD"
#define NORMALCELLCOLOR   "#FFDDDD"

// constructor
SpectraSTSimScores::SpectraSTSimScores() :
  dot(0.0),
  delta(0.0),
  dotBias(0.0),
  precursorMzDiff(0.0),
  hitsNum(0),
  hitsMean(0.0),
  hitsStDev(0.0),
  fval(0.0),
  firstNonHomolog(0) {
  
}

// copy constructor
SpectraSTSimScores::SpectraSTSimScores(SpectraSTSimScores& other) {
  *this = other;	
}

// destructor
SpectraSTSimScores::~SpectraSTSimScores() {
}

// assignment operator
SpectraSTSimScores& SpectraSTSimScores::operator=(SpectraSTSimScores& other) {	
  this->dot = other.dot;
  this->delta = other.delta;
  this->dotBias = other.dotBias;
  this->precursorMzDiff = other.precursorMzDiff;
  this->hitsNum = other.hitsNum;
  this->hitsMean = other.hitsMean;
  this->hitsStDev = other.hitsStDev;
  this->fval = other.fval;
  this->firstNonHomolog = other.firstNonHomolog;
  return (*this);
}

// calcFval - calculates the F value. This functional form of the F value is determined by trial and error on 
// many datasets, and designed to work with PeptideProphet.
double SpectraSTSimScores::calcOldFval() {
  
  if (dot < 0.00001) {
    fval = 0.0;
    return (0.0);
  }
  
  fval = 0.6 * dot + 0.4 * delta / dot;
  
  if (fval > 0.4 && hitsNum < 20) {
    fval = 0.8 * dot;
  }
  
  // impose dot bias penalty
  if (fval > 0.4) {
    if (dotBias < 0.09) {
      fval -= 0.12;
    } else if (dotBias > 0.35 && dotBias <= 0.40) {
      fval -= 0.12;
    } else if (dotBias > 0.40 && dotBias <= 0.45) {
      fval -= 0.18;
    } else if (dotBias > 0.45) {
      fval -= 0.24;
    }
  }
  return (fval);
  
}

// calcFval - calculates the F value. This functional form of the F value is determined by trial and error on 
// many datasets, and designed to work with PeptideProphet.
double SpectraSTSimScores::calcFval(double fractionDelta) {
  
  if (dot < 0.00001) {
    fval = -0.00001;
    return (-0.00001);
  }
  
  fval = (1 - fractionDelta) * dot + fractionDelta * delta / dot;
  
  if (fval > 0.4 && hitsNum < 20) {
    fval = (1 - 0.5 * fractionDelta) * dot;
  }
  
  imposeDotBiasPenalty();
  
  return (fval);
  
}

double SpectraSTSimScores::imposeDotBiasPenalty() {
  // impose dot bias penalty
  if (dotBias < 0.09) {
    fval -= 0.12;
  } else if (dotBias > 0.32 && dotBias <= 0.35) {
    fval -= (dotBias - 0.32) * 4.0;
  } else if (dotBias > 0.35 && dotBias <= 0.45) {
    fval -= (0.12 + (dotBias - 0.35) * 1.2);
  } else if (dotBias > 0.45) {
    fval -= 0.24;
  }
  
   if (fval <= 0.0) {
    fval = -0.00001;
  }
  
  return (fval);
}


// printFixedWidth - prints the scores in fixed width windows on a line. (For .txt outputs)
void SpectraSTSimScores::printFixedWidth(ofstream& fout) {
  fout.width(10);
  fout.precision(3);		
  fout << left << dot;
  fout.width(10);
  fout.precision(3);
  fout << left << delta;
  
  stringstream deltaRkss;
  deltaRkss << '[' << firstNonHomolog << ']';
  fout.width(6);
  fout << left << deltaRkss.str();
  
  fout.width(10);
  fout.precision(3);
  fout << left << dotBias;
  fout.width(10);
  fout.precision(3);
  fout << left << precursorMzDiff;
  fout.width(10);
  fout.precision(3);
  fout << left << hitsNum;
  fout.width(10);
  fout.precision(3);
  fout << left << hitsMean;
  fout.width(10);
  fout.precision(3);
  fout << left << hitsStDev;
  fout.width(10);
  fout.precision(3);
  fout << left << fval;
  
  
}

void SpectraSTSimScores::printHeaderFixedWidth(ofstream& fout) {
  
  fout.width(10);
  fout << "Dot";
  fout.width(10);
  fout << "Delta";
  fout.width(6);
  fout << "DelRk";
  fout.width(10);
  fout << "DBias";
  fout.width(10);
  fout << "MzDiff";
  fout.width(10);
  fout << "#Cand";
  fout.width(10);
  fout << "MeanDot";
  fout.width(10);
  fout << "SDDot";
  fout.width(10);
  fout << "Fval";
  
}

// printTabDelimited - prints the scores, separated by tabs, on a line. (For .xls outputs)
void SpectraSTSimScores::printTabDelimited(ofstream& fout) {
  
  fout.precision(3);		
  fout << left << dot << '\t';
  fout.precision(3);
  fout << left << delta << '\t';
  fout.precision(2);
  fout << left << '[' << firstNonHomolog << ']' << '\t';
  fout.precision(3);
  fout << left << dotBias << '\t';
  fout.precision(3);
  fout << left << precursorMzDiff << '\t';
  fout.precision(3);
  fout << left << hitsNum << '\t';	
  fout.precision(3);
  fout << left << hitsMean << '\t';	
  fout.precision(3);
  fout << left << hitsStDev << '\t';
  fout.precision(3);
  fout << left << fval << '\t';
  
  
}

void SpectraSTSimScores::printHeaderTabDelimited(ofstream& fout) {
  
  fout << "Dot" << '\t';
  fout << "Delta" << '\t';
  fout << "DelRk" << '\t';
  fout << "DBias" << '\t';
  fout << "MzDiff" << '\t';
  fout << "#Cand" << '\t';
  fout << "MeanDot" << '\t';
  fout << "SDDot" << '\t';
  fout << "Fval" << '\t';
  
}


void SpectraSTSimScores::printHtml(ofstream& fout) {
  
  fout.precision(3);
  fout << "  <TD BGCOLOR=\"" << NORMALCELLCOLOR << "\"><TT>" << fixed << fval << "</TT></TD>" << endl; 
  fout.precision(3);
  fout << "  <TD BGCOLOR=\"" << NORMALCELLCOLOR << "\"><TT>" << fixed << dot << "</TT></TD>" << endl;
  fout.precision(3);
  fout << "  <TD BGCOLOR=\"" << NORMALCELLCOLOR << "\"><TT>" << fixed << delta << "</TT></TD>" << endl;
  fout.precision(3);
  fout << "  <TD BGCOLOR=\"" << NORMALCELLCOLOR << "\"><TT>" << fixed << dotBias << "</TT></TD>" << endl;
  fout.precision(4);
  fout << "  <TD BGCOLOR=\"" << NORMALCELLCOLOR << "\"><TT>" << fixed << showpos << precursorMzDiff << noshowpos << "</TT></TD>" << endl;
  
}

void SpectraSTSimScores::printHeaderHtml(ofstream& fout) {
  
  fout << "  <TH BGCOLOR=\"" << HEADERCELLCOLOR << "\"><TT>" << "Fval" << "</TT></TH>" << endl; 
  fout << "  <TH BGCOLOR=\"" << HEADERCELLCOLOR << "\"><TT>" << "Dot" << "</TT></TH>" << endl;
  fout << "  <TH BGCOLOR=\"" << HEADERCELLCOLOR << "\"><TT>" << "Delta" << "</TT></TH>" << endl;
  fout << "  <TH BGCOLOR=\"" << HEADERCELLCOLOR << "\"><TT>" << "DBias" << "</TT></TH>" << endl;
  fout << "  <TH BGCOLOR=\"" << HEADERCELLCOLOR << "\"><TT>" << "MzDiff" << "</TT></TH>" << endl;
  
  
}

// printPepXML - prints the scores as pepXML elements. (For .pepXML outputs)
void SpectraSTSimScores::printPepXML(ofstream& fout) {
  
  fout.precision(3);		
  fout << "<search_score name=\"dot\" value=\"" << dot << "\"/>" << endl;	
  fout.precision(3);		
  fout << "<search_score name=\"delta\" value=\"" << delta << "\"/>" << endl;	
  fout.precision(3);		 
  fout << "<search_score name=\"dot_bias\" value=\"" << dotBias << "\"/>" << endl;	
  fout.precision(3);		
  fout << "<search_score name=\"precursor_mz_diff\" value=\"" << precursorMzDiff << "\"/>" << endl;	
  fout.precision(3);		
  fout << "<search_score name=\"hits_num\" value=\"" << hitsNum << "\"/>" << endl;	
  fout.precision(3);		
  fout << "<search_score name=\"hits_mean\" value=\"" << hitsMean << "\"/>" << endl;	
  fout.precision(3);		
  fout << "<search_score name=\"hits_stdev\" value=\"" << hitsStDev << "\"/>" << endl;	
  fout.precision(3);		
  fout << "<search_score name=\"fval\" value=\"" << fval << "\"/>" << endl;	
  fout.precision(2);
  fout << "<search_score name=\"first_non_homolog\" value=\"" << firstNonHomolog << "\"/>" << endl;
  
  
	
}

