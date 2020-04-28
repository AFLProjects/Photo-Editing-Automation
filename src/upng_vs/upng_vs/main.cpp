#include "ImageProcessing.h"

using namespace std;

#pragma warning(disable:4996)

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
		
		if (command.substr(0, 4) == "edit")
		{
			/*!!!!!!!!!!!!!!!!!!
			Do optymizacji jeszcze pare zmian zrobie :

				-Nie bede uzywal clasy Color, bo spowalnia , i uzywa 3 razy wiecej pamieci
				uzyje po prostu unsigned char ktory mam od poczatku

				-Do sortowania sprobobuje nie uzywac vector<vector<Color*>> i tez nie uzywac .push_back bo tez spowalnia
				tylko po prostu unsigned char[...] , ale nie wiem jeszcze jak bedzie indexowane bo
				jak sie robi R+G+B to czasami dwa colory maja to sama sume. Wiec si overriduja
				zrobilem inny algorithm ktory uzywal tylko unsigned char[...] ale gorzej sie white balansowalo ale byl szybszy

			!!!!!!!!!!!!!!!!!!!*/

			/*Get file path*/
			std::string path = command.substr(5, command.length());
			cout << endl;

			unsigned int w = 0;
			unsigned int h = 0;

			/*Decode image from file*/
			std::vector<unsigned char> image = ImageProcessing::decodeImage(path.c_str(), w, h);
			std::vector<unsigned char> *imgPtr = &image;

			auto start = std::chrono::high_resolution_clock::now(); /*timer*/

			/*Size of image in memory*/
			float sizeMb = 3.0 * 32.0 * w * h / 1000.0 / 1000.0;
			/*3x times because Color class has 3 floats so it is 96bit per col instead of 32bit , so 3 times more*/

			cout << "Image decoded from [" << path << "][w:" << w << ";h:" << h << "][" << sizeMb << "MB]\n\n";

			int imageSize = w * h;

			/*Convert from char to Color class*/
			vector<Color*> imageCols(imageSize);
			ImageProcessing::DecodePixels(imageCols, imgPtr, imageSize);
		
			/*Sort colors using lookup table where the index is R+G+B*/
			vector<vector<Color*>> sortedColorsLookupTable(766); /*Multiple color can have the same sum*/

			/*Count of occurences for each sum value*/
			int colCount[766];
			memset(colCount, 0, sizeof(colCount));

			ImageProcessing::SortPixels(sortedColorsLookupTable, colCount, imageCols, imageSize);

			/*Image white pixel*/
			Color referenceWhite = Color(0, 0, 0, Color::ColorType::HSV);
			ImageProcessing::FindWhiteColor(referenceWhite, sortedColorsLookupTable, imageSize);

			/*Apply changes using the found white reference color*/
			ImageProcessing::ApplyChanges(imgPtr, imageCols, referenceWhite, imageSize);

			auto elapsed = std::chrono::high_resolution_clock::now() - start;
			long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

			printf("\nTotal time of execution(without decoding/encoding) : ");
			cout << milliseconds << endl;

			cout << "Saving image...\n";

			/*Save image*/
			ImageProcessing::encodeImage(("updated_" + path).c_str(), *imgPtr, w, h);

			cout << "Image Saved!\n" << endl;
		}

	}

	std::getchar();
	return 0;

}