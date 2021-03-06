// P6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <unordered_map>
#include "fileRead.h"
#include "VectorHash.h"

using namespace std;

typedef char T;

int languageToIndex(string language)
{
	if (language == "danish")
		return 0;
	else if (language == "english")
		return 1;
	else if (language == "french")
		return 2;
	else if (language == "italian")
		return 3;
	else if (language == "latin")
		return 4;
	else
		return 5;
}

int main(int argc, char *argv[])
{
	// n delta senLength
	vector<string> languages = {"danish", "english", "french", "italian", "latin", "sweedish"};
	vector<T> tokens;

	
	int n = stoi(argv[1]);
	double delta = stod(argv[2]);
	int senLength = stoi(argv[3]);

	unordered_map<string, unordered_map<int, unordered_map<vector<T>, double>>> languageDatabases;
	unordered_map<string, double> languageNValue;

	for (int k = 0; k < languages.size(); k++)
	{
		unordered_map<int, unordered_map<vector<T>, double>> nGramDatabases;
		string fileName = languages[k] + "1.txt";

		tokens.clear();
		read_tokens(fileName, tokens, false);
		languageNValue[languages[k]] = double(tokens.size());

		for (int j = 0; j < n; j++)
		{
			for (int i = 0; i <= tokens.size() - (n - j); i++)
			{
				vector<T> nGram(n - j);     // for temporarily storing tokens to go into next n-gram
											// Take next n tokens read from the input file 

				for (unsigned int z = 0; z < (n - j); z++) // put next n tokens into vector temp
				{
					nGram[z] = tokens[i + z];

				}

				if (nGramDatabases[n - j].count(nGram) == 0) // nGram is not in the database yet, insert it with count 1
				{
					nGramDatabases[n - j][nGram] = 1;
				}
				else // nGram is already in the database, increase its count by 1
					nGramDatabases[n - j][nGram] = nGramDatabases[n - j][nGram] + 1;
			}

		}
		languageDatabases[languages[k]] = nGramDatabases;
	}

	unordered_map<string, unordered_map<vector<T>,string>> sentances;
	double V = 256.0;
	for (int k = 0; k < languages.size(); k++)
	{

		string fileName = languages[k] + "2.txt";
		tokens.clear();
		read_tokens(fileName, tokens, false);

		unordered_map<vector<T>, string> temp;

		for (int j = 0; j < (tokens.size() - senLength); j++)
		{
			vector<T> nGram(senLength);
			for (unsigned int i = 0; i < (senLength); i++) // put next n tokens into vector temp
			{
				nGram[i] = tokens[j + i];
			}

			double maxProb = -1 * DBL_MAX;
			vector<T> prevWords;
			double numerator, denominator;
			string language = "none";
			double logSumHold = 0;
			for (auto it = languageDatabases.begin(); it != languageDatabases.end(); ++it)
			{
				double logSum = 0.0;
				for (int i = 0; i < nGram.size(); i++)
				{
					if (prevWords.size() == 0)
					{
						denominator = languageNValue[it->first] + (pow(V, prevWords.size() + 1)*delta);
					}
					else
					{
						double tempNum = double(it->second[prevWords.size()][prevWords]) + delta;
						double tempB = pow(V, prevWords.size());
						double tempDen = languageNValue[it->first] + tempB*delta;

						denominator = double(tempNum / tempDen);
					}

					prevWords.push_back(nGram[i]);
					if (prevWords.size() > n)
						prevWords.erase(prevWords.begin());

					if (prevWords.size() == 1)
					{
						numerator = double(it->second[prevWords.size()][prevWords]) + delta;
					}
					else
					{
						double tempNum = double((it->second)[prevWords.size()][prevWords]) + delta;
						double tempB = pow(V, prevWords.size());
						double tempDen = languageNValue[it->first] + tempB*delta;
						numerator = double(tempNum / tempDen);

					}
					if (numerator == 0 || denominator == 0)
					{
						i = nGram.size();
						logSum = -1 * DBL_MAX;
					}
					else
					{

						logSum += log(numerator / denominator);
					}
					if (prevWords.size() > (n - 1))
						prevWords.erase(prevWords.begin());
				}

				if (logSum >= maxProb)
				{
					maxProb = logSum;
					language = it->first;
				}
			}
			temp[nGram] = language;
		}

		sentances[languages[k]] = temp;
		temp.clear();

	}
	double inCorrect = 0.0;
	double total = 0.0;
	vector<vector<double>> confusionMatrix(6);
	int languageIndex = 0;
	for (auto langSentIt = sentances.begin(); langSentIt != sentances.end(); ++langSentIt)
	{
		//string language = langSentIt->first;
		languageIndex = languageToIndex(langSentIt->first);
		vector<double > temp(6);
		for (auto sentIt = (langSentIt->second).begin(); sentIt != ((langSentIt->second)).end(); ++sentIt)
		{
			total++;
			if ((langSentIt->first) == (sentIt->second))
			{
				temp[languageIndex]++;
			}
			else
			{
				inCorrect++;
				temp[languageToIndex(sentIt->second)]++;
			}
			
		}

		confusionMatrix[languageIndex] = (temp);
		temp.clear();
	}

	cout << endl << (inCorrect / total)*100.0 << endl << endl;

	for (int i = 0; i < confusionMatrix.size(); i++)
	{
		for (int j = 0; j < confusionMatrix[i].size(); j++)
		{
			cout << confusionMatrix[i][j] << " ";
		}
		cout << endl;
	}

	string endingString;
	cout << endl << endl << "Press any key and enter to exit" << endl;
	cin >> endingString;

    return 0;
}

