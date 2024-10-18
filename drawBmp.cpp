#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <windows.h>

class ReadBMP {
private:
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    std::vector<std::vector<uint8_t>> data;
    std::ifstream file;

public:

    ReadBMP() {};
    ~ReadBMP() { closeBMP(); }

    bool openBMP(const std::string& fileName) {
        file.open(fileName, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        if (!file.read(reinterpret_cast<char*>(&fileHeader), sizeof(BITMAPFILEHEADER)) ||
            !file.read(reinterpret_cast<char*>(&infoHeader), sizeof(BITMAPINFOHEADER))) {
            return false;
        }

        if (fileHeader.bfType != 0x4D42) {
            return false;
        }

        if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
            return false;
        }

        infoHeader.biHeight = abs(infoHeader.biHeight);
        data.resize(infoHeader.biHeight, std::vector<uint8_t>(infoHeader.biWidth * 3));

        for (int i = 0; i < infoHeader.biHeight; ++i) {
            for (int j = 0; j < infoHeader.biWidth; ++j) {
                file.read(reinterpret_cast<char*>(&data[i][j * 3]), 3);
            }
            // Filling in each row to align to a multiple of 4 bytes
            file.ignore((4 - (infoHeader.biWidth * 3) % 4) % 4);
        }

        return true;
    }

    void displayBMP() {
        for (int i = infoHeader.biHeight - 1; i >= 0; --i) {
            for (size_t j = 0; j < data[i].size(); j += 3) {
                if (data[i][j] == 0 && data[i][j + 1] == 0 && data[i][j + 2] == 0) {
                    std::cout << " "; // Black
                }
                else if (data[i][j] == 255 && data[i][j + 1] == 255 && data[i][j + 2] == 255) {
                    std::cout << "#"; // White
                }
            }
            std::cout << std::endl;
        }
    }

    void closeBMP() {
        data.clear();
        data.shrink_to_fit();
        file.close();
    }
};

int main(int argc, char* argv[]) {

    std::string filePath = argv[1];

    ReadBMP reader;

    if (reader.openBMP(filePath)) {
        reader.displayBMP();
    }
    else {
        std::cout << "Error opening the BMP file" << std::endl;
    }
    return 0;
}