#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

using namespace std;

map<string, int> wordAmountMap;
mutex mutexMap;

const string wordName = "Word";
const string amountName = "Amount";
const string fileNotFoundMessage = "File %$FILE_NAME$% not found";
const string fileNameReplaceToken = "%$FILE_NAME$%";

void writeWordAmountIntoMap(string fileName)
{
	string line;

	ifstream fileStream(fileName);
	if (fileStream.is_open())
	{
		while (getline(fileStream, line))
		{
			//split line into single words
			istringstream buffer(line);
			istream_iterator<string> start(buffer), end;
			vector<string> words(start, end);

			//insert into map
			for (vector<string>::iterator it = words.begin(); it != words.end(); it++)
			{
				mutexMap.lock();
				if (wordAmountMap.count(*it) == 0)
				{
					wordAmountMap[*it] = 1;
				}
				else
				{
					wordAmountMap[*it] ++;
				}
				mutexMap.unlock();
			}
		}
		fileStream.close();
	}
	else
	{
		//file not found
		string outputMessage = fileNotFoundMessage;
		cout << outputMessage.replace(outputMessage.find(fileNameReplaceToken), fileNameReplaceToken.length(), fileName) << endl;
	}
}

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		vector<thread> threads;

		//create threads
		for (int i = 1; i < argc; i++)
		{
			thread thread(writeWordAmountIntoMap, argv[i]);
			threads.push_back(move(thread));
		}

		//wait for all threads to finish
		for (auto& thread : threads)
		{
			thread.join();
		}

		if (!wordAmountMap.empty())
		{
			//find the longest word, for better display of results
			string longestWord = wordName;
			for (auto& entry : wordAmountMap)
			{
				if (entry.first.length() > longestWord.length())
				{
					longestWord = entry.first;
				}
			}

			cout << wordName << string(longestWord.length() - wordName.length() + 1, ' ') << amountName << endl;

			for (auto& entry : wordAmountMap)
			{
				cout << entry.first << string(longestWord.length() - entry.first.length() + 1, ' ') << entry.second << endl;
			}
		}
	}

	return 0;
}