#include <iostream>
#include <fstream>
#include <vector>


//Указывает компилятору на то, что каждый элемент структуры должен быть выровнен на границу в памяти, равную 1 байту. 
//Это означает, что каждый элемент будет размещен без дополнительных байтов для выравнивания.
#pragma pack(1)

//Заголовок BMP файла содержит информацию о структуре и параметрах изображения
struct BMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset; // сдвиг до пикселей (потому что сначала идет header)
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t dataSize; // размер пиксельных данных
    int32_t horizontalRes;
    int32_t verticalRes;
    uint32_t colors;
    uint32_t importantColors;
};

#pragma pack (1)

// Функция для применения фильтра Гаусса к изображению
void GaussianFilter(std::vector<unsigned char>& pixels, int width, int height) {
    std::vector<unsigned char> filteredPixels(pixels.size());

    // двумерный массив kernel размером 5x5, который представляет собой матрицу (ядро) для применения фильтра Гаусса
    //который будет использоваться при свертке с пикселями изображения для выполнения размытия с использованием фильтра Гаусса.
    double kernel[5][5] = {
        {0.003, 0.013, 0.022, 0.013, 0.003},
        {0.013, 0.059, 0.097, 0.059, 0.013},
        {0.022, 0.097, 0.159, 0.097, 0.022},
        {0.013, 0.059, 0.097, 0.059, 0.013},
        {0.003, 0.013, 0.022, 0.013, 0.003}
    };

    //при применении фильтра будут учитываться пиксели вокруг центрального пикселя в квадрате размером 5x5 (от -2 до +2 относительно центра).
    int radius = 2;
    
    // Применить фильтр к каждому пикселю изображения


    // Внешний цикл выполняется по вертикальным пикселям изображения, начинается с пикселя, который находится на расстоянии radius, то есть 2, 
    // от верхней границы изображения 
    // цикл не будет выполняться на пикселях внутри радиуса от верхней или нижней границы изображения
    for (int y = radius; y < height - radius; y++) {
        //Внутренний цикл выполняется по горизонтальным пикселям изображения. Он начинается с пикселя, который находится на расстоянии radius,  
        //то есть 2, от левой границы изображения
        //цикл не будет выполняться на пикселях внутри радиуса от левой или правой границы изображения
        for (int x = radius; x < width - radius; x++) {
            //Переменные sumR, sumG и sumB будут использоваться для накопления значений красного, зеленого и синего каналов пикселей 
            //внутри заданного радиуса для дальнейшей фильтрации изображения
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            
            // Свернуть ядро с изображением

            //данный внешний цикл перебирает вертикальные пиксели изображения, исключая пиксели внутри радиуса от верхней и нижней границ изображения
            for (int j = -radius; j <= radius; j++) {
                //Данный внутренний цикл перебирает горизонтальные пиксели изображения, исключая пиксели внутри радиуса от левой и правой границ изображения
                for (int i = -radius; i <= radius; i++) {
                    
                    // данная переменная вычисляет смещение для доступа к пикселю в векторе
                    // смещение рассчитывается на основе координаты (x + i, y + j) и ширины изображения (width). 
                    // Здесь учтено, что пиксели изображения могут иметь три канала: красный (R), зеленый (G) и синий (B).
                    int pixelOffset = ((y + j) * width + (x + i)) * 3;
                    double weight = kernel[j + radius][i + radius];

                    sumR += pixels[pixelOffset] * weight;
                    sumG += pixels[pixelOffset + 1] * weight;
                    sumB += pixels[pixelOffset + 2] * weight;
                }
            }

            // завершение обработки одного пикселя и сохранение результатов в новом векторе filteredPixels

            //Здесь вычисляется смещение (offset) в векторе filteredPixels, где будут храниться результаты обработки текущего пикселя.
            //Поскольку каждый пиксель представлен тройкой значений (R, G, B), умножение на 3 используется для правильного индексирования вектора
            int offset = (y * width + x) * 3;

            //Значение переменной sumR, sumG, sumB которое представляет сумму взвешенных значений красного/зеленого/синего
            // канала для текущего пикселя, сохраняется в векторе filteredPixels в соответствующем красном/зеленом/синем канале для текущего пикселя.
            filteredPixels[offset] = static_cast<unsigned char>(sumR);
            filteredPixels[offset + 1] = static_cast<unsigned char>(sumG);
            filteredPixels[offset + 2] = static_cast<unsigned char>(sumB);
        }
    }
    // теперь значение filteredPixels будет хранить результаты фильтрации (или любой другой обработки), 
    // и оно будет применено к переменной pixels, таким образом, pixels теперь будет содержать отфильтрованные пиксели изображения.
    pixels = filteredPixels;
}

// Функция поворота изображения на 90 градусов по часовой стрелки
//int width: Ширина исходного изображения (количество пикселей в строке).
//int height: Высота исходного изображения (количество пикселей в столбце).
void rotateImage90Degrees(std::vector<unsigned char>& pixels, int width, int height) {

    //Ссылка на вектор, который содержит пиксели изображения, которое нужно повернуть
    std::vector<unsigned char> rotatedPixels(pixels.size());
    
    // Выполнить итерацию по изображению и поверните его
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            //смещение в векторе pixels, которое вычисляется на основе текущих координат x и y
            int srcOffset = (y * width + x) * 3;

            //смещение в векторе rotatedPixels, где будут скопированы значения из pixels. Это смещение вычисляется так, 
            //чтобы пиксели из исходного изображения переместились в новое изображение после его поворота на 90 градусов. 
            int destOffset = ((width - x - 1) * height + y) * 3;

            //Значения красного, зеленого и синего каналов из исходного пикселя 
            //копируются в соответствующие каналы нового пикселя в rotatedPixels с использованием destOffset.
            rotatedPixels[destOffset] = pixels[srcOffset];
            rotatedPixels[destOffset + 1] = pixels[srcOffset + 1];
            rotatedPixels[destOffset + 2] = pixels[srcOffset + 2];
        }
    }

    pixels = rotatedPixels;
}




int main() {
    //setlocale() - это функция которая определяет, как некоторые функции в программе будут вести себя в отношении языка и 
    //культурных особенностей

    //LC_ALL указывает на установку локали для всех категорий (например, LC_CTYPE, LC_TIME, LC_COLLATE, и так далее), 
    //что означает, что вся  программа будет работать с русской локалью.
    setlocale(LC_ALL, "RU");

    // Открыть мой BMP-файл
    
    std::ifstream inputFile("bmp02.bmp", std::ios::binary);

    // Прочитать заголовок BMP
    BMPHeader header;
    
    //вызываю функцию read чтобы использовать для считывания данных из inputFile
    //чтобы функция read могла записать байты заголовка в буфер, представленный указателем на char

    //reinterpret_cast<char*>(&header) это преобразование указателя на структуру header в указатель на char
    //чтобы функция read могла записать байты заголовка в буфер, представленный указателем на char

    //sizeof(header): Это размер данных, которые должны быть считаны из потока
    //функция read считает из потока байты, необходимые для заполнения этой структуры.
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

    
    int width = header.width;
    int height = header.height;

    // header.bitsPerPixel переменная (или свойство), которая представляет глубину цвета изображения в битах
    
    // header.bitsPerPixel / 8 Это выражение преобразует глубину цвета в битах в количество байт, необходимых для хранения одного пикселя
    
    // width * header.bitsPerPixel / 8: Это выражение вычисляет общее количество байт, необходимых для хранения всей строки изображения
    
    //width * header.bitsPerPixel / 8 + 3: Здесь к общему размеру строки добавляется 3. 
    //Это может быть необходимо для выравнивания памяти или других технических соображений, 
    //чтобы учесть дополнительные байты, которые могут быть выделены в памяти.
    int rowSize = (width * header.bitsPerPixel / 8 + 3) & (~3);
    
    //Создайть вектор для хранения пикселей изображения
    std::vector<unsigned char> pixels(rowSize * height);
    
    //Считать данные изображения

    //inputFile.read() этот метод используется для чтения данных из файла

    //reinterpret_cast<char*>(pixels.data()): Это выражение выполняет приведение типа данных. Оно берет указатель на начало данных в векторе 
    //pixels, преобразует его в указатель на char (байты), и передает этот указатель функции read()

    //pixels.size() возвращает размер вектора pixels, который равен общему количеству байт
    inputFile.read(reinterpret_cast<char*>(pixels.data()), pixels.size());
    inputFile.close();

    // Повернуть изображение на 90 градусов против часовой стрелки
    rotateImage90Degrees(pixels, width, height);
    header.width = height;
    header.height = width;
    
    // Обновите размер файла в заголовке BMP

    
    //sizeof(header) возвращает размер структуры header в байтах.

    //.size() - это метод или функция, возвращающая количество элементов в контейнере pixels
    header.fileSize = sizeof(header) + pixels.size();

    
    
    
    // Открыть повернутый входной BMP-файл

    //std::ofstream outputFile("finish.bmp", std::ios::binary); - 
    //Эта строка создает объект outputFile класса std::ofstream, который предназначен для записи данных в файл.

   
    std::ofstream outputFile("finish.bmp", std::ios::binary);

    //outputFile.write(reinterpret_cast<char*>(&header), sizeof(header)); - 
    //Этот код записывает данные из структуры (или объекта) header в файл outputFile. reinterpret_cast используется 
    //для приведения указателя на header к типу char*, что позволяет записать данные в бинарном формате. 

    outputFile.write(reinterpret_cast<char*>(&header), sizeof(header));
    outputFile.write(reinterpret_cast<char*>(pixels.data()), pixels.size());
    outputFile.close();

    std::cout << "BMP файл перевернут в finish.bmp" << std::endl;

    std::ifstream rotatedInputFile("finish.bmp", std::ios::binary);
    
    // Считать заголовок BMP повернутого изображения
    BMPHeader rotatedHeader;
    rotatedInputFile.read(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader));

    // Извлечь ширину и высоту повернутого изображения
    int rotatedWidth = rotatedHeader.width;
    int rotatedHeight = rotatedHeader.height;

    //Вычислить размер строки повернутого изображения
    int rotatedRowSize = (rotatedWidth * rotatedHeader.bitsPerPixel / 8 + 3) & (~3);

    // Создайть вектор для хранения повернутых пикселей изображения
    std::vector<unsigned char> rotatedPixels(rotatedRowSize * rotatedHeight);

    // Считывает пиксельные данные повернутого изображения
    //reinterpret_cast<char*>(rotatedPixels.data()
    //Это может быть необходимо, чтобы считать байты данных как последовательность символов типа char
    rotatedInputFile.read(reinterpret_cast<char*>(rotatedPixels.data()), rotatedPixels.size());
    rotatedInputFile.close();

    // Применить фильтр Гаусса к повернутому изображению
    GaussianFilter(rotatedPixels, rotatedWidth, rotatedHeight);

    // Создать выходной BMP-файл для отфильтрованного изображения
    std::ofstream outputGaussianFile("finishG.bmp", std::ios::binary);

    // Записать заголовок BMP и отфильтрованные пиксельные данные в выходной файл
    outputGaussianFile.write(reinterpret_cast<char*>(&rotatedHeader), sizeof(rotatedHeader));
    outputGaussianFile.write(reinterpret_cast<char*>(rotatedPixels.data()), rotatedPixels.size());
    outputGaussianFile.close();

    std::cout << "Фильтр Гаусса готов" << std::endl;

    return 0;
}
