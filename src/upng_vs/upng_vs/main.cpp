#include "ImageProcessing.h"

using namespace std;

/*SETTINGS*/

struct Settings
{
public:
	/*
	Index - Property
	0 - Color Balance
	1 - White Intensity
	2 - Black Intensity
	3 - Shadow Intensity
	4 - Vividity Min
	5 - Vividity Max
	*/
	float data[6];

	Settings(string path)
	{
		std::ifstream file(path);
		std::string str;
		int k = 0;
		while (std::getline(file, str)) {
			data[k] = std::stof(str);
			k++;
		}
	}
	Settings() { memset(data, -1, sizeof(data)); }

	void DisplaySettings()
	{
		cout << "\nColor Balance : " << data[0] << endl;
		cout << "White Intensity : " << data[1] << endl;
		cout << "Black Intensity : " << data[2] << endl;
		cout << "Shadow Intensity : " << data[3] << endl;
		cout << "Vividity Min : " << data[4] << endl;
		cout << "Vividity Max : " << data[5] << endl << endl;
	}
};
/*SETTINGS END*/



Settings currentSettings = Settings("settings.ini");
int main() {
	/*<==FAST IO==>*/
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	/*<==TITLE==>*/
	const char *title =
		"     ----     `---.      .---.       .---.      .---`       `-----..`            `--------.            .://:-`          "
		"    `MMMM`    -MMMm      -NMMM:     -NMMM:      hMMM/       oMMMMMMMMNh/`        :MMMMMMMMd         /hNMMMMMMMms-       "
		"    `MMMM`    -MMMm       :MMMN.   `mMMM+       hMMM/       oMMMmssymMMMN:       :MMMNssss+       .dMMMMmhyhNMMMMy      "
		"    `MMMM`    -MMMm        oMMMd`  hMMMs        hMMM/       oMMMs    +MMMN.      :MMMm`````      `mMMMh.     :mMMMy     "
		"    `MMMM`    -MMMm         yMMMy oMMMh         hMMM/       oMMMs     mMMM+      :MMMMMMMMo      /MMMN        :MMMM`    "
		"    `MMMM`    :MMMm          dMMMyMMMd`         hMMM/       oMMMs    `NMMM:      :MMMNyyyy/      -MMMM-       oMMMm     "
		"     mMMMs`  .hMMMy          `mMMMMMN.          hMMM/       oMMMs  .+mMMMh       :MMMm            sMMMNs:.`./hMMMM:     "
		"     -NMMMMNNMMMMy`           .NMMMN-           hMMM/       oMMMMMMMMMMm+        :MMMMMMMMd        /mMMMMMMMMMMMh-      "
		"      `/sdmNmmho.              :hhh:            shhh:       /hhhhhhhs+-          -hhhhhhhhs          -ohdmNmdy+.        ";
	cout << title << "\n by Adrian Fluder" << endl;

	/*<==Commands==>*/
	const char *commands =
		"\nCommands :               "
		"\n\t-settings              //Shows the settings"
		"\n\t-settings -s           //Set setting manually"
		"\n\t-settings -a		    //Set the settings automatically from videos in <dir>"
		"\n\t-edit					//Edits the videos/images in <dir>\n\n";
	cout << commands << endl;

	while (true)
	{

		cout << ">>";
		std::string command;
		std::getline(std::cin, command);

		if (command == "settings")
		{
			if (!ImageProcessing::fileExists("settings.ini"))
			{
				std::ofstream file("settings.ini");
				file << "1\n100\n0\n50\n50\n70" << std::endl;
				file.close();
				currentSettings = Settings("settings.ini");
			}
			currentSettings.DisplaySettings();
		}
		else if (command == "settings -s")
		{

			string cb, wi, bi, si, vmi, vma;
			cout << "\nColor Balance : " << endl;
			std::getline(std::cin, cb);
			cout << "White Intensity : " << endl;
			std::getline(std::cin, wi);
			cout << "Black Intensity : " << endl;
			std::getline(std::cin, bi);
			cout << "Shadow Intensity : " << endl;
			std::getline(std::cin, si);
			cout << "Vividity Min : " << endl;
			std::getline(std::cin, vmi);
			cout << "Vividity Max : " << endl;
			std::getline(std::cin, vma);
			std::string output = cb + "\n" + wi + "\n" + bi + "\n" + si + "\n" + vmi + "\n" + vma;

			std::ofstream file("settings.ini");
			file << output << std::endl;
			file.close();

			cout << "\nNew Settings" << endl;
			currentSettings = Settings("settings.ini");
			currentSettings.DisplaySettings();
		}
		else if (command == "settings -a")
		{
			cout << "Not implemented!\n\n";
		}
		else if (command.substr(0, 4) == "edit")
		{
			std::string path = command.substr(5, command.length());
			cout << endl;

			unsigned int w = 0;
			unsigned int h = 0;
			/*Decode*/
			std::vector<unsigned char> image = ImageProcessing::decodeImage(path.c_str(), w, h);
			std::vector<unsigned char> *imgPtr = &image;

			float sizeMb = 32.0 * w * h / 1000.0 / 1000.0;

			cout << "Image decoded from [" << path << "][w:" << w << ";h:" << h << "][" << sizeMb << "MB]\n\n";

			auto start = std::chrono::high_resolution_clock::now();

			int imageSize = w * h;

			vector<Color*> imageCols(imageSize);


			ImageProcessing::DecodePixels(imageCols, imgPtr, imageSize, true);

			auto elapsed_extracting = std::chrono::high_resolution_clock::now() - start;
			float extractionTime = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_extracting).count() / 1000.0;

			int ExtractingkBytesProcessed = sizeof(vector<Color*>) + (sizeof(Color) * imageCols.size() / 1000.0);
			float ExtractingkBytesPerSec = ExtractingkBytesProcessed / extractionTime;

			cout << "[" << extractionTime << "s][" << ExtractingkBytesPerSec << "kB/s] " << "Extracting colors (100%)[##########################]" << endl;

			int SortingWorkFinishedOld = 0;
			int SortingWorkFinished = 0;

			Color empty = Color(-1, -1, -1, Color::ColorType::RGB);

			int precision = 1000;
			vector<Color*> sortedColors((255* precision)+1,&empty);

			for (int i = 0; i < imageSize; i++)
			{
				Color *col = (imageCols[i]);

				//int index = round((0.299 * (*col).data[0] + 0.587 * (*col).data[1] + 0.114 * (*col).data[2])*precision);
				int index = round((0.2126 * (*col).data[0] + 0.7152 * (*col).data[1] + 0.0722 * (*col).data[2])*precision);
				sortedColors[index] = col;

				SortingWorkFinishedOld = SortingWorkFinished;
				SortingWorkFinished = (int)round(((i*1.0) / (imageSize * 1.0))*22.0);

				if (SortingWorkFinishedOld != SortingWorkFinished) {
					cout << "Sorting colors (" << SortingWorkFinished / 22.0*100.0 << "%)[";
					int workNotFinished = 22 - SortingWorkFinished;

					cout << string(SortingWorkFinished, '#');
					cout << string(workNotFinished, '-');

					cout << "]\r";
				}
			}

			auto elapsed_sorting = std::chrono::high_resolution_clock::now() - start;
			float sortingTime = (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_sorting).count() / 1000.0) - extractionTime;

			int SortingkBytesProcessed = (sizeof(vector<vector<Color*>>) + sizeof(Color*) * imageCols.size()) / 1000.0;
			float SortingBytesPerSec = SortingkBytesProcessed / sortingTime;

			cout << "[" << sortingTime << "s][" << SortingBytesPerSec << "kB/s] " << "Sorting colors (100%)[##########################]" << endl;

			int whiteCount = round(0.8 / 100.0*imageSize);
			int currentCount = 0;
			//vector<Color*> selectedWhiteColors(whiteCount);

			
			int WhiteRefWorkFinishedOld = 0;
			int WhiteRefWorkFinished = 0;

			Color referenceWhite = Color(0, 0, 0, Color::ColorType::HSV);

			for (int i = 255*precision; i > -1; i--)
			{
				if (currentCount < whiteCount)
				{
					if (sortedColors[i] != &empty) {

						Color col = (*sortedColors[i]);

						currentCount++;

						col.RGBtoHSV();

						referenceWhite.data[0] += col.data[0] / whiteCount;
						referenceWhite.data[1] += col.data[1] / whiteCount;
						referenceWhite.data[2] += col.data[2] / whiteCount;

						col.HSVtoRGB();

						WhiteRefWorkFinishedOld = WhiteRefWorkFinished;
						WhiteRefWorkFinished = (int)round(((currentCount*1.0) / (whiteCount * 1.0))*22.0);

						if (WhiteRefWorkFinishedOld != WhiteRefWorkFinished) {
							cout << "Calculating Avrg. White color(HSV) (" << WhiteRefWorkFinished / 22.0*100.0 << "%)[";
							int workNotFinished = 22 - WhiteRefWorkFinished;

							cout << string(WhiteRefWorkFinished, '#');
							cout << string(workNotFinished, '-');

							cout << "]\r";
						}
					}
				}
				else
				{
					break;
				}
			}

			/*
			for (int i = 765; i > -1; i--)
			{
				if (currentCount < whiteCount)
				{
					//int size = sortedColors[i].size();
					int size = colCount[i];
					if (size > 0)
					{
						for (int j = 0; j < size; j++)
						{
							if (currentCount < whiteCount) {
								Color col = *(sortedColors[i + j]);

								currentCount++;

								col.RGBtoHSV();

								referenceWhite.data[0] += col.data[0] / whiteCount;
								referenceWhite.data[1] += col.data[1] / whiteCount;
								referenceWhite.data[2] += col.data[2] / whiteCount;

								col.HSVtoRGB();

								WhiteRefWorkFinishedOld = WhiteRefWorkFinished;
								WhiteRefWorkFinished = (int)round(((currentCount*1.0) / (whiteCount * 1.0))*22.0);

								if (WhiteRefWorkFinishedOld != WhiteRefWorkFinished) {
									cout << "Calculating Avrg. White color(HSV) (" << WhiteRefWorkFinished / 22.0*100.0 << "%)[";
									int workNotFinished = 22 - WhiteRefWorkFinished;

									cout << string(WhiteRefWorkFinished, '#');
									cout << string(workNotFinished, '-');

									cout << "]\r";
								}
							}
							else
							{
								break;
							}
						}
					}
				}
				else
				{
					break;
				}


			}
			*/

			auto elapsed_whiteref = std::chrono::high_resolution_clock::now() - start;
			float whiterefTime = (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_whiteref).count() / 1000.0) - extractionTime - sortingTime;

			cout << "[" << whiterefTime << "s] " << "Calculating Avrg. White color(HSV) (100%)[##########################]" << endl;

		//	printf("\nreference White : \nH:%f S:%f V:%f\n", referenceWhite.data[0], referenceWhite.data[1], referenceWhite.data[2]);
			referenceWhite.HSVtoRGB();
			referenceWhite = Color(1 / (referenceWhite.data[0] / 255.0), 1 / (referenceWhite.data[1] / 255.0), 1 / (referenceWhite.data[2] / 255.0), Color::ColorType::RGB);
		//	printf("\nreference White : \nR:%f G:%f B:%f\n", referenceWhite.data[0], referenceWhite.data[1], referenceWhite.data[2]);

			int ApplyWorkFinishedOld = 0;
			int ApplyWorkFinished = 0;

			for (int i = 0; i < imageSize; i++)
			{
				Color* col;
				col = imageCols[i];

				if (referenceWhite.data[0] > 3)
					referenceWhite.data[0] = 3;
				if (referenceWhite.data[1] > 3)
					referenceWhite.data[1] = 3;
				if (referenceWhite.data[2] > 3)
					referenceWhite.data[2] = 3;

				(*col).data[0] *= referenceWhite.data[0];
				(*col).data[1] *= referenceWhite.data[1];
				(*col).data[2] *= referenceWhite.data[2];

				if ((*col).data[0] >= 255)
					(*col).data[0] = 255;

				if ((*col).data[1] >= 255)
					(*col).data[1] = 255;

				if ((*col).data[2] >= 255)
					(*col).data[2] = 255;

				(*imgPtr)[i * 4] = (char)round((*col).data[0]);
				(*imgPtr)[(i * 4) + 1] = (char)round((*col).data[1]);
				(*imgPtr)[(i * 4) + 2] = (char)round((*col).data[2]);

				ApplyWorkFinishedOld = ApplyWorkFinished;
				ApplyWorkFinished = (int)round(((i*1.0) / (imageSize * 1.0))*22.0);

				if (ApplyWorkFinishedOld != ApplyWorkFinished) {
					cout << "Applying changes (" << ApplyWorkFinished / 22.0*100.0 << "%)[";
					int workNotFinished = 22 - ApplyWorkFinished;

					cout << string(ApplyWorkFinished, '#');
					cout << string(workNotFinished, '-');

					cout << "]\r";
				}
			}

			auto elapsed_apply = std::chrono::high_resolution_clock::now() - start;
			float applyTime = (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_apply).count() / 1000.0) - extractionTime - sortingTime - whiterefTime;

			cout << "[" << applyTime << "s] " << "Applying changes (100%)[##########################]" << endl;


			auto elapsed = std::chrono::high_resolution_clock::now() - start;
			long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

			printf("\nTotal time of execution(without decoding/encoding) : ");
			cout << milliseconds << endl;

			ImageProcessing::encodeImage(("updated_" + path).c_str(), *imgPtr, w, h);
		}

	}

	std::getchar();
	return 0;

}

