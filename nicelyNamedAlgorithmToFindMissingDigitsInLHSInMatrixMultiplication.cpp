#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

int main() {
	string LHSfilename, RHSfilename, RESfilename;
	cout << "Enter LHS matrix filename: ";
	cin >> LHSfilename;
	cout << "Enter RHS matrix filename: ";
	cin >> RHSfilename;
	cout << "Enter RES matrix filename: ";
	cin >> RESfilename;
	ifstream LHSfile, RHSfile, RESfile;
	ofstream output;
	string outputFilename;
	cout << "Enter output filename: ";
	cin >> outputFilename;
	output.open(outputFilename);
	LHSfile.open(LHSfilename);
	RHSfile.open(RHSfilename);
	RESfile.open(RESfilename);
	if(LHSfile.fail()){cout << "The LHS file cannot be opened";}
	else if(RHSfile.fail()){cout << "The RHS file cannot be opened";}
	else if(RESfile.fail()){cout << "The RES file cannot be opened";} // 8-24 taking the names of the three matrixes, opening them, and checking for failure, also declaring the output variable
	else {
		int LHSRowNum, LHSColumnNum, RHSRowNum, RHSColumnNum, RESRowNum, RESColumnNum;
		string LHSSize, RHSSize, RESSize;
		getline(LHSfile, LHSSize);
		istringstream LHSStream(LHSSize);
		LHSStream >> LHSRowNum >> LHSColumnNum;
		getline(RHSfile, RHSSize);
		istringstream RHSStream(RHSSize);
		RHSStream >> RHSRowNum >> RHSColumnNum;
		getline(RESfile, RESSize);
		istringstream RESStream(RESSize);
		RESStream >> RESRowNum >> RESColumnNum;
		vector<vector<int>> LHS (LHSRowNum, vector<int>(LHSColumnNum));
		vector<vector<int>> RHS (RHSRowNum, vector<int>(RHSColumnNum));
		vector<vector<int>> RES (RESRowNum, vector<int>(RESColumnNum)); // 25-39 getting the sizes of the matrixes from each file and creating the matrixes
		string line;
		bool error = false;
		if (RHSColumnNum != 1 || RESColumnNum != 1) {error = true;}
		int i = 0, missingDigCount = 0;
		int num;
		while(getline(LHSfile, line)){
			istringstream lineStream(line);
			for (int o = 0; o < LHSColumnNum; o++){
				if(lineStream.rdbuf()->in_avail() != 0){lineStream >> num;}
				else {error = true;}
				if (num > 9 || num == 0 || num < -1){error = true;}
				if (num==-1){missingDigCount++;}
				LHS[i][o]=num;
			}
			string clear;
			lineStream >> clear;
			if(lineStream.rdbuf()->in_avail() != 0){error = true;} // a line I admittedly copied from stack overflow, it checks whether the stream still has elements and ticks error if there are
			i++;
		}
		i = 0;
		while(getline(RHSfile, line)){
			istringstream lineStream(line);
			for (int o = 0; o < RHSColumnNum; o++){
				if(lineStream.rdbuf()->in_avail() != 0){lineStream >> num;}
				else {error = true;}
				if (num > 9 || num < 1){error = true;}
				RHS[i][o]=num;
			}
			string clear;
			lineStream >> clear;
			if(lineStream.rdbuf()->in_avail() != 0){error = true;}
			i++;
		}
		i = 0;
		while(getline(RESfile, line)){
			istringstream lineStream(line);
			for (int o = 0; o < RESColumnNum; o++){
				if(lineStream.rdbuf()->in_avail() != 0){lineStream >> num;}
				else {error = true;}
				if (num < 0){error = true;}
				RES[i][o]=num;
			}
			string clear;
			lineStream >> clear;
			if(lineStream.rdbuf()->in_avail() != 0){error = true;}
			i++;
		} 
		vector<int> missingDigits(missingDigCount, 1), missingDigitRows(missingDigCount), missingDigitCols(missingDigCount); // 40-87 fills the matrixes with the values from the files while checking for errors and counts the number of missing digits, 
																															// then makes a vector to store the missing digits
		if (error) {output << "Error"; output.close();}
		else {
			int idx = missingDigCount - 1, idx2 = missingDigCount - 1; //idx is used for keeping track of the next index which is to be filled in the missingDigits vector
																	   //idx2 is used for keeping track of the next index of the missingDigitRows and missingDigitCols vectors, which notes the rows and columns of the missing digits. 
			for(int i = LHSRowNum-1; i >= 0; i--){
				bool areProperDigits = true;						   // areProperDigits is true when the digits of a row are [1, 9] and false when otherwise
				vector <int> missingDigIdx;							   // missingDigIdx notes the indexes of the missing digits in each row 
				int total = RES[i][0], lowestVarRHS = 10, lowestVarRHSI = -1;  // total keeps track of the remaining number that should be allocated to digits to keep the equation correct (if total==0 then the math checks out)
				for (int o = 0; o < LHSColumnNum; o++){
					if(LHS[i][o] != -1) {
						total -= (LHS[i][o] * RHS[o][0]);
					}
					else{
						total -= (RHS[o][0]); 
						missingDigIdx.push_back(o);
					}
				} // counts the total and notes the indexes of the missing digits on missingDigIdx
				for (int o = LHSColumnNum - 1; o >= 0; o--){
					if(LHS[i][o]==-1){
						missingDigitRows[idx2] = i;
						missingDigitCols[idx2] = o;
						idx2--;
					}
				}
				for (int i = 0; i < missingDigIdx.size(); i++){
					if(RHS[missingDigIdx[i]][0]<lowestVarRHS){lowestVarRHS=RHS[missingDigIdx[i]][0]; lowestVarRHSI=i;}
				}
				for (int o = missingDigIdx.size()-1; o >= 0; o--){
					for (int p = 9; p > 1 && total; p--){
						if(p*RHS[missingDigIdx[o]][0] <= total){
							total -= (p-1)*RHS[missingDigIdx[o]][0];
							missingDigits[idx] = p;
							break;
						}
					} 
				idx--;
				} // assigns the missing digits with numbers
				int rowSum = 0;
						for (int o = 0; o < missingDigIdx.size(); o++){
							rowSum += missingDigits[idx + o + 1] * RHS[missingDigIdx[o]][0];
						}
						for (int o = 0; o < LHSColumnNum; o++){
							if (LHS[i][o]!=-1){rowSum+=LHS[i][o]*RHS[o][0];}
						}
						for (int o = 0; o < missingDigIdx.size(); o++){
							if(missingDigits[idx + o + 1] > 9 || missingDigits[idx + o + 1] < 1){areProperDigits = false; break;}
							if(o == missingDigIdx.size()-1){areProperDigits=true;}
						}
						total = RES[i][0] - rowSum; // recounts the total
					cout << "total: " << total << endl;
			
				do {
					while(total != 0){
						for (int o = 0; o < missingDigIdx.size(); o++){
								if(total / RHS[missingDigIdx[o]][0] != 0){
									int change = total / RHS[missingDigIdx[o]][0];
									if(missingDigits[idx + o + 1] + change > 9){change = 9 - missingDigits[idx + o + 1];}
									else if(missingDigits[idx + o + 1] + change < 1){change = -1*(missingDigits[idx + o + 1]-1);}
									missingDigits[idx + o + 1] += change;
									total -= change * RHS[missingDigIdx[o]][0];
								}
						} // checks whether there are any RHS values that are bigger than total and can reduce the total (unlikely to happen because of previous measures but still good to have just in case) 
						for (int o = 0; o < missingDigIdx.size(); o++){
							for (int p = o; p < missingDigIdx.size(); p++){
								if (p!=o) {
									int difference = RHS[missingDigIdx[p]][0] - RHS[missingDigIdx[o]][0];
									for(int a = 1; a < missingDigits[o + idx + 1] && total!=0; a++){
										for(int b = 1; b < missingDigits[p + idx + 1] && total!=0; b++){
  											int difference = RHS[missingDigIdx[o]][0]*a - RHS[missingDigIdx[p]][0]*b;
											if(difference > 0 && total >= difference && missingDigits[o + idx + 1]<10-a && missingDigits[p + idx + 1]>b){total-=difference; missingDigits[o + idx + 1]+=a; missingDigits[p + idx + 1]-=b;}
											else if(difference < 0 && total >= -1*difference && missingDigits[o + idx + 1]>a && missingDigits[p + idx + 1]<10-b){total-=difference*-1; missingDigits[o + idx + 1]-=a; missingDigits[p + idx + 1]+=b;}}}
									if(difference > 0){
										if(total / difference != 0 && total>0){
											missingDigits[idx + p + 1] ++;
											missingDigits[idx + o + 1] --;
											total -= difference;
										}
										else if(total / difference != 0 && total<0){
											missingDigits[idx + p + 1] --;
											missingDigits[idx + o + 1] ++;
											total += difference;
										}
									}
									else if(difference < 0){
										difference *= -1;
										if(total / difference != 0 && total > 0){
											missingDigits[idx + o + 1] ++;
											missingDigits[idx + p + 1] --;
											total -= difference;
										}
										else if(total / difference != 0 && total < 0){
											missingDigits[idx + o + 1] --;
											missingDigits[idx + p + 1] ++;
											total += difference;
										}
									}
								}
							}
						} // tries to find numbers to switch around to lower total
					} // fix to make total == 0
					
					for (int o = 0; o < missingDigIdx.size(); o++){
						int counter = 0, altLowestRHS = lowestVarRHS, altLowestRHSI = lowestVarRHSI, lowestRHSdiff = 10, lowestRHSdiffI1 = 0, lowestRHSdiffI2 = 0; // counter keeps track of how many times over the needed amount the digit was reduced,
																																								   // altLowestRHS records the lowest RHS value available without changing lowestVarRHS, altLowestRHSI does the same for its index
																																								   // lowestRHSdiff finds the lowest RHS difference that can be used to reduce excess, lowestRHSdiffI1 and 2 record the bigger and smaller RHSs respectively
						while(missingDigits[idx + o + 1] > 9){
							int excess = (missingDigits[idx + o + 1]-9) * (RHS[missingDigIdx[o]][0]); // excess (much like total) keeps track of the number that needs to be redistributed to the digits for the equation to be correct
								while(excess>0){
									while(excess >= altLowestRHS){
										if(missingDigits[idx + lowestVarRHSI + 1] >= 9 || missingDigits[idx + altLowestRHSI + 1] >= 9 || lowestVarRHSI==o){
											altLowestRHS = 999;
											for (int p = 0; p < missingDigIdx.size(); p++){
												if(RHS[missingDigIdx[p]][0]<=altLowestRHS && missingDigits[idx + p + 1] < 9 && p!=o){altLowestRHS=RHS[missingDigIdx[p]][0]; altLowestRHSI=p;}
											}
										} // rechecks the altLowestRHS value and changes the indexes accordingly
										for (int p = 0; p < missingDigIdx.size() && missingDigits[idx + o + 1] > 9; p++){
											if(excess / RHS[missingDigIdx[p]][0] > 0 && p!=o){
												if(missingDigits[idx + p + 1] < 9){
													missingDigits[idx + p + 1]++;
													excess -= RHS[missingDigIdx[p]][0];
												}
											}
										}
									} // reduces the excess as much as it can by increasing other digits
										while(excess >= lowestRHSdiff && missingDigits[idx + lowestRHSdiffI1 + 1] >= 9 && missingDigits[idx + lowestRHSdiffI2 + 1] <= 1){
											if((lowestRHSdiffI1==0 && lowestRHSdiffI2==0) || missingDigits[idx + lowestRHSdiffI1 + 1] >= 9 || missingDigits[idx + lowestRHSdiffI2 + 1] <= 1){
												for(int p = 0; p < missingDigIdx.size(); p++){
													for(int x = 0; x < missingDigIdx.size(); x++){
														if (x!=p && x!=o && p!=o) {
															int difference = RHS[missingDigIdx[x]][0] - RHS[missingDigIdx[p]][0];
															if(difference > 0){if(difference < lowestRHSdiff){if(missingDigits[idx + x + 1]<9 && missingDigits[idx + p + 1]>1){lowestRHSdiff = difference; lowestRHSdiffI1=x; lowestRHSdiffI2=p;}}}
															if(difference < 0){if(difference < -1*lowestRHSdiff){if(missingDigits[idx + p + 1]<9 && missingDigits[idx + x + 1]>1){lowestRHSdiff = -1*difference; lowestRHSdiffI1=p; lowestRHSdiffI2=x;}}}
														}
													}
												}
											} // rechecks the lowestRHSdiff value and changes the indexes accordingly
											if(missingDigIdx.size()==2){
												lowestRHSdiff = RHS[missingDigIdx[1]][0] - RHS[missingDigIdx[0]][0];
												lowestRHSdiffI1 = 1; lowestRHSdiffI2 = 0;
												if(lowestRHSdiff<0){lowestRHSdiff*=-1; lowestRHSdiffI1=0; lowestRHSdiffI2=1;}
											} // in case there are 2 variables in a vector it uses a more simple calculation to get the lowestRHSdiff
											for (int p = 0; p < missingDigIdx.size() && missingDigits[idx + o + 1] > 9; p++){
												for(int x = 0; x < missingDigIdx.size(); x++){
													if (x!=p && x!=o && p!=o) {
														int difference = RHS[missingDigIdx[x]][0] - RHS[missingDigIdx[p]][0];
														if(difference > 0){
															if(excess / difference != 0 && missingDigits[idx + x + 1] < 9){
																missingDigits[idx + x + 1] ++;
																missingDigits[idx + p + 1] --;
																excess -= difference;
															}
														}
														else if(difference < 0){
															difference *= -1;
															if(excess / difference != 0 && missingDigits[idx + p + 1] < 9){
																missingDigits[idx + p + 1] ++;
																missingDigits[idx + x + 1] --;
																excess -= difference;
															}
														}
													}
												}
										} // tries to reduce the excess by switching around digits
										}
										if(excess==0){missingDigits[idx + o + 1]=(9-counter); break;}
										else if (excess < lowestVarRHS && missingDigits[idx + lowestVarRHSI + 1] < 9){excess += RHS[missingDigIdx[o]][0];counter++;}
										else if (excess < altLowestRHS && missingDigits[idx + altLowestRHSI + 1] < 9){excess += RHS[missingDigIdx[o]][0];counter++;} // sets the value of the digit to (9-counter) if excess == 0, increases the excess and counter otherwise
								}
						}} // fix to make lhs < 10
					for (int o = 0; o < missingDigIdx.size(); o++){
						int counter = 0, altLowestRHS = lowestVarRHS, altLowestRHSI = lowestVarRHSI, lowestRHSdiff = 10, lowestRHSdiffI1 = 0, lowestRHSdiffI2 = 0; // the variables here work basically the same way as the above code,
																																								   // except counter which now records how many times over the needed amount the digit was increased
						while(missingDigits[idx + o + 1] < 1){
							int excess = (1-missingDigits[idx + o + 1]) * RHS[missingDigIdx[o]][0]; // excess also works the same way
							while(excess>0){
								while(excess >= altLowestRHS){
									if(missingDigits[idx + lowestVarRHSI + 1] <= 1 || missingDigits[idx + altLowestRHSI + 1] <= 1 || lowestVarRHSI==o){
										altLowestRHS = 999;
										for (int p = 0; p < missingDigIdx.size(); p++){
											if(RHS[missingDigIdx[p]][0]<=altLowestRHS && missingDigits[idx + p + 1] < 9 && p!=o){altLowestRHS=RHS[missingDigIdx[p]][0]; altLowestRHSI=p;}
										}
									}
									for (int p = 0; p < missingDigIdx.size() && missingDigits[idx + o + 1] < 1; p++){
										if(excess / RHS[missingDigIdx[p]][0] > 0 && p!=o){
											if(missingDigits[idx + p + 1] > 0){
												missingDigits[idx + p + 1]--;
												excess -= RHS[missingDigIdx[p]][0];
											}
										}
									}
								} // reduces the excess as much as it can by reducing other elements
								
									if((lowestRHSdiffI1==0 && lowestRHSdiffI2==0) || missingDigits[idx + lowestRHSdiffI1 + 1] <= 9 || missingDigits[idx + lowestRHSdiffI2 + 1] <= 1){
										for(int p = 0; p < missingDigIdx.size(); p++){
											for(int x = 0; x < missingDigIdx.size(); x++){
												if (x!=p && x!=o && p!=o) {
													int difference = RHS[missingDigIdx[x]][0] - RHS[missingDigIdx[p]][0];
													if(difference > 0){if(difference < lowestRHSdiff){if(missingDigits[idx + x + 1]<9 && missingDigits[idx + p + 1]>1){lowestRHSdiff = difference; lowestRHSdiffI1=x; lowestRHSdiffI2=p;}}}
													if(difference < 0){if(difference < -1*lowestRHSdiff){if(missingDigits[idx + p + 1]<9 && missingDigits[idx + x + 1]>1){lowestRHSdiff = -1*difference; lowestRHSdiffI1=p; lowestRHSdiffI2=x;}}}
												}
											}
										}
									}
									while(excess >= lowestRHSdiff && missingDigits[idx + lowestRHSdiffI1 + 1] >= 9 && missingDigits[idx + lowestRHSdiffI2 + 1] <= 1){
										if(missingDigIdx.size()==2){
												lowestRHSdiff = RHS[missingDigIdx[1]][0] - RHS[missingDigIdx[0]][0];
												lowestRHSdiffI1 = 1; lowestRHSdiffI2 = 0;
												if(lowestRHSdiff<0){lowestRHSdiff*=-1; lowestRHSdiffI1=0; lowestRHSdiffI2=1;}
											}
										for (int p = 0; p < missingDigIdx.size() && missingDigits[idx + o + 1] > 9; p++){
											for(int x = 0; x < missingDigIdx.size(); x++){
												if (x!=p && x!=o && p!=o) {
													int difference = RHS[missingDigIdx[x]][0] - RHS[missingDigIdx[p]][0];
													if(difference > 0){
														if(excess / difference != 0 && missingDigits[idx + x + 1] < 9 && missingDigits[idx + p + 1] > 1){
															missingDigits[idx + x + 1] ++;
															missingDigits[idx + p + 1] --;
															excess -= difference;
														}
													}
													else if(difference < 0){
														difference *= -1;
														if(excess / difference != 0 && missingDigits[idx + p + 1] < 9 && missingDigits[idx +  x + 1] > 1){
															missingDigits[idx + p + 1] ++;
															missingDigits[idx + x + 1] --;
															excess -= difference;
														}
													}
												}
											}
									}
									}
									if(excess==0){missingDigits[idx + o + 1]=(1+counter); break;}
									else if (excess < lowestVarRHS && missingDigits[idx + lowestVarRHSI + 1] < 9){excess += RHS[missingDigIdx[o]][0];counter++;}
									else if (excess < altLowestRHS && missingDigits[idx + altLowestRHSI + 1] < 9){excess += RHS[missingDigIdx[o]][0];counter++;} // same thing with the above function except it sets the value to (1+counter)
						} 
						}} // fix to make lhs > 0
					for(int m = 0; m < 1; m++){
						for (int o = 0; o < missingDigIdx.size()-1 && missingDigIdx.size(); o++){
						if(missingDigits[idx + o + 1] > 1){
							int smallestWorkingNum = missingDigits[idx + o + 1], triedNum = missingDigits[idx + o + 1]-1, altLowestRHS = lowestVarRHS, altLowestRHSI = lowestVarRHSI, lowestRHSdiff = 10, lowestRHSdiffI1 = 0, lowestRHSdiffI2 = 0;
							// smallestWorkingNum keeps track of the smalles triedNum that worked. triedNum is the current digit that is being checkedl. The rest of the variables work similar to how they were used in the above code 
								while(triedNum>0){
									vector <int> missingDigitsCopy(missingDigIdx.size()); // keeps a copy of how the currently worked on row of missingDigits looks like and changes it instead of missingDigits itself
									for (int p = 0; p < missingDigIdx.size(); p++){
										missingDigitsCopy[p] = missingDigits[idx + p + 1];
									} // copies the needed row of missingDigits to missingDigitsCopy
									int excess = (missingDigitsCopy[o]-triedNum) * (RHS[missingDigIdx[o]][0]); // keeps track of the number that needs to be redistributed for the digit reduction to make sense
									if(excess>0){
										do{
											if(missingDigitsCopy[lowestVarRHSI] >= 9 || missingDigitsCopy[altLowestRHSI] >= 9 || lowestVarRHSI<=o){
												altLowestRHS = 999;
												for (int p = o; p < missingDigIdx.size(); p++){
													if(RHS[missingDigIdx[p]][0]<=altLowestRHS && missingDigitsCopy[p] < 9 && p!=o){altLowestRHS=RHS[missingDigIdx[p]][0]; altLowestRHSI=p;}
												}
											}	// rechecks the altLowestRHS and reduces index accordingly
											for (int p = o; p < missingDigIdx.size(); p++){
												if(excess / RHS[missingDigIdx[p]][0] > 0 && p!=o){
													if(missingDigitsCopy[p] < 9){
														missingDigitsCopy[p]++;
														excess -= RHS[missingDigIdx[p]][0];
													}
												}
											}
										} while(excess >= altLowestRHS); // lowers excess as much as possible by increasing other digits in missingDigitsCopy
											do {
												lowestRHSdiffI1=0;
												lowestRHSdiffI2=0;
												lowestRHSdiff=999;
												if((lowestRHSdiffI1==0 && lowestRHSdiffI2==0) || missingDigitsCopy[lowestRHSdiffI1] >= 9 || missingDigitsCopy[lowestRHSdiffI2] <= 1 || lowestRHSdiffI1 == o || lowestRHSdiffI2 == o){
													if (missingDigIdx.size()-o > 2){
														for(int p = o; p < missingDigIdx.size(); p++){
															for(int x = o; x < missingDigIdx.size(); x++){
																if (x!=p && x!=o && p!=o) {
																	for(int a = 1; a < missingDigitsCopy[x] && excess!=0; a++){
																		for(int b = 1; b < missingDigitsCopy[p] && excess!=0; b++){
  																			int difference = RHS[missingDigIdx[x]][0]*a - RHS[missingDigIdx[p]][0]*b;
																			if(difference > 0 && excess >= difference && missingDigitsCopy[x]<10-a && missingDigitsCopy[p]>b){excess-=difference; missingDigitsCopy[x]+=a; missingDigitsCopy[p]-=b;}
																		}
																	}
																	int difference = RHS[missingDigIdx[x]][0] - RHS[missingDigIdx[p]][0];
																	if(difference > 0){if(difference <= lowestRHSdiff && missingDigitsCopy[x]<9 && missingDigitsCopy[p]>1){lowestRHSdiff = difference; lowestRHSdiffI1=x; lowestRHSdiffI2=p;}}
																	if(difference < 0){if(-1*difference <= lowestRHSdiff && missingDigitsCopy[p]<9 && missingDigitsCopy[x]>1){lowestRHSdiff = -1*difference; lowestRHSdiffI1=p; lowestRHSdiffI2=x;}}
																} // works the same way other "difference" for loops work, except able to take possible multiplication of RHSs into account, and being more thorough at the cost of speed
															}
														}
													}
												} // 
													for(int p = o; p < missingDigIdx.size() && excess != 0; p++){
														for(int x = o; x < missingDigIdx.size(); x++){
															if (x!=p && x!=o && p!=o) {
																int difference = RHS[missingDigIdx[x]][0] - RHS[missingDigIdx[p]][0];
																if(difference > 0){
																	if(excess / difference != 0 && missingDigitsCopy[x] < 9 && missingDigitsCopy[p] > 1){
																		missingDigitsCopy[x] ++;
																		missingDigitsCopy[p] --;
																		excess -= difference;
																	}
																}
																else if(difference < 0){
																	difference *= -1;
																	if(excess / difference != 0 && missingDigitsCopy[p] < 9 && missingDigitsCopy[x] > 1){
																		missingDigitsCopy[p] ++;
																		missingDigitsCopy[x] --;
																		excess -= difference;
																	}
																}
															}
														}
													} // in case the previous for loop didn't work this "difference" loop takes into effect
											} while (excess >= lowestRHSdiff);
										if(excess==0){smallestWorkingNum=triedNum;}
									}
									triedNum--; // ticks triedNum down to check the lower digit
								}
								int excess = (missingDigits[idx + o + 1]-smallestWorkingNum) * (RHS[missingDigIdx[o]][0]);
									while(excess>0){
										do {
											if(missingDigits[idx + lowestVarRHSI + 1] >= 9 || missingDigits[idx + altLowestRHSI + 1] >= 9 || lowestVarRHSI<=o){
											altLowestRHS = 999;
												for (int p = o; p < missingDigIdx.size(); p++){
													if(RHS[missingDigIdx[p]][0]<=altLowestRHS && missingDigits[idx + p + 1] < 9 && p!=o){altLowestRHS=RHS[missingDigIdx[p]][0]; altLowestRHSI=p;}
												}
											}
											for (int p = o; p < missingDigIdx.size(); p++){
												if(excess / RHS[missingDigIdx[p]][0] > 0 && p!=o){
													if(missingDigits[idx + p + 1] < 9){
														missingDigits[idx + p + 1]++;
														excess -= RHS[missingDigIdx[p]][0];
													}
												}
											}
										} while(excess >= altLowestRHS);
											do{
												if((lowestRHSdiffI1==0 && lowestRHSdiffI2==0) || missingDigits[idx + lowestRHSdiffI1 + 1] >= 9 || missingDigits[idx + lowestRHSdiffI2 + 1] <= 1 || lowestRHSdiffI1 == o || lowestRHSdiffI2 == o){
													if(missingDigIdx.size()-o==2){
														lowestRHSdiff = 999;
													}
												else{
													lowestRHSdiff = 999;
													for(int p = o; p < missingDigIdx.size(); p++){
														for(int x = o; x < missingDigIdx.size(); x++){
															if (x!=p && x!=o && p!=o) {
																for(int a = 1; a < missingDigits[idx + x + 1] && excess != 0; a++){
																		for(int b = 1; b < missingDigits[idx + p + 1] && excess != 0; b++){
  																			int difference = RHS[missingDigIdx[x]][0]*a - RHS[missingDigIdx[p]][0]*b;
																			if(difference > 0 && excess >= difference && missingDigits[idx + x + 1]<10-a && missingDigits[idx + p + 1]>b){excess-=difference; missingDigits[idx + x + 1]+=a; missingDigits[idx + p + 1]-=b;}
																		}
																	}
																	int difference = RHS[missingDigIdx[x]][0] - RHS[missingDigIdx[p]][0];
																	if(difference > 0){if(difference <= lowestRHSdiff && missingDigits[idx + x + 1]<9 && missingDigits[idx + p + 1]>1){lowestRHSdiff = difference; lowestRHSdiffI1=x; lowestRHSdiffI2=p;}}
																	if(difference < 0){if(-1*difference <= lowestRHSdiff && missingDigits[idx + p + 1]<9 && missingDigits[idx + x + 1]>1){lowestRHSdiff = -1*difference; lowestRHSdiffI1=p; lowestRHSdiffI2=x;}}
															}
														}
													}
												}}
													for(int p = o; p < missingDigIdx.size() && excess != 0; p++){
														for(int x = o; x < missingDigIdx.size(); x++){
															if (x!=p && x!=o && p!=o) {
																int difference = RHS[missingDigIdx[x]][0] - RHS[missingDigIdx[p]][0];
																if(difference > 0){
																	if(excess / difference != 0 && missingDigits[idx + x + 1] < 9 && missingDigits[idx + p + 1] > 1){
																		missingDigits[idx + x + 1] ++;
																		missingDigits[idx + p + 1] --;
																		excess -= difference;
																	}
																}
																else if(difference < 0){
																	difference *= -1;
																	if(excess / difference != 0 && missingDigits[idx + p + 1] < 9 && missingDigits[idx +  x + 1] > 1){
																		missingDigits[idx + p + 1] ++;
																		missingDigits[idx + x + 1] --;
																		excess -= difference;
																	}
																}
															}
														}
													}
											} while (excess >= lowestRHSdiff);
			//								}
												missingDigits[idx + o + 1] = smallestWorkingNum;
										} // takes the smallestWorkingNum and makes the changes to lower the given digit, the code itself is basically the same as the one above
								}
					}} // changes the digits so that the final line of digits is as small as possible, meaning leftmost digits in a line are smaller	
					    int rowSum = 0;
						for (int o = 0; o < missingDigIdx.size(); o++){
							rowSum += missingDigits[idx + o + 1] * RHS[missingDigIdx[o]][0];
						}
						for (int o = 0; o < LHSColumnNum; o++){
							if (LHS[i][o]!=-1){rowSum+=LHS[i][o]*RHS[o][0];}
						}
						for (int o = 0; o < missingDigIdx.size(); o++){
							if(missingDigits[idx + o + 1] > 9 || missingDigits[idx + o + 1] < 1){areProperDigits = false; break;}
							if(o == missingDigIdx.size()-1){areProperDigits=true;}
						}
						total = RES[i][0] - rowSum; // recounts the total
					cout << "total: " << total << endl;
				} while (total != 0 || !areProperDigits); 
				missingDigIdx.clear();
			} // clears the list of indexes of the missing digits in a line
			for (int i = 0 ; i < missingDigCount; i++){output << missingDigitRows[i] << " " << missingDigitCols[i] << " " << missingDigits[i] << endl;} // pushes the results to output.txt
			output.close();
		}
		
	}
	return 0;
}