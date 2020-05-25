#include <iostream>
#include <Windows.h>


#define BOUNDARY_ID 9
#define EMPTY_ID 0
#define X_PADDING 34
#define Y_PADDING 2

std::wstring tetromino[7];

// Columns of Console
int nScreenWidth = 80;
// Rows of Console
int nScreenHeight = 30;

int nFieldWidth = 12;
int nFieldHeight = 18;
// initialize the field as an array of unsigned chars,
// unsigned chars 0 is empty space, other ints are different shapes, and a boundary wall
// not statically allocated, will allocate dynamically.
unsigned char* pField = nullptr;


// Rotate 90 degress times R
int Rotate(int px, int py, int r)
{
    // keep r in range of 0-3 inclusive with mod
    switch (r % 4)
    {
    // calculate the new position for the given point
    case 0: return py * 4 + px;
    case 1: return 12 + py - (px * 4);
    case 2: return 15 - (py * 4) - px;
    case 3: return 3 - py + (px * 4);
    default: return 0;   
    }
}

bool DoesPieceFit(int nTetrimino, int nRotation, int nPosX, int nPosY)
{
    // if anything fails return false

    // iterate over every position in the tetromino
    for (int px = 0; px < 4; px++)
    {
        for (int py = 0; py < 4; py++)
        {
            // Get index into piece using any set rotation
            int pi = Rotate(px, py, nRotation);

            // Get index into field
            // using the position of the piece, and the offset within the piece
            int fi = (nPosY + py) * nFieldWidth + (nPosX + px);
            
            // Collision Detection
            if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
            {
                if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
                {
                    // if the tetromino has an X occupancy and the field has anything
                    // but empty space, return false on first hit
                    if (tetromino[nTetrimino][pi] == L'X' && pField[fi] != 0)
                    {
                        return false;
                    }
                }
            }
        }
    }


    // if everything passes return true
    return true;
}

int main()
{
    std::cout << "Hello World!" << std::endl;

    // Construct Tetrominos
    // Bar 
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    // Box
    tetromino[1].append(L"....");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L"....");
    // Left Z
    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");
    // Right Z
    tetromino[3].append(L"..X.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".X..");
    tetromino[3].append(L"....");
    // L
    tetromino[4].append(L".X..");
    tetromino[4].append(L".X..");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"....");
    // Backwards L
    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"....");
    // T
    tetromino[6].append(L"..X.");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L"..X.");
    tetromino[6].append(L"....");

    // Initialize the play field
    pField = new unsigned char[nFieldWidth * nFieldHeight];
    for (int x = 0; x < nFieldWidth; x++)
    {
        for (int y = 0; y < nFieldHeight; y++)
        {
            // set boundaries to BOUNDARY_ID, otherwise set to EMPTY_ID
            pField[y * nFieldWidth + x] = (
                x == 0 || 
                x == nFieldWidth - 1 ||
                y == nFieldHeight-1
                ) 
                ? BOUNDARY_ID : EMPTY_ID;
        }
    }

    // Create the Windows Command Prompt View
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);


    // Set Buffer Size
    //_COORD consoleSize;
    //consoleSize.X = nScreenWidth;
    //consoleSize.Y = nScreenHeight;
    //SetConsoleScreenBufferSize(hConsole, consoleSize);

    ////// Set Window Size
    //_SMALL_RECT windowRect;
    //windowRect.Top = 0;
    //windowRect.Left = 0;
    //windowRect.Bottom = nScreenHeight - 1;
    //windowRect.Right = nScreenWidth - 1;
    //SetConsoleWindowInfo(hConsole, TRUE, &windowRect);

    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Game Logic
    // Game Loop
    bool bGameOver = false;

    // Game State
    int nCurrentPiece = 1;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2; // center the falling piece
    int nCurrentY = 0; // at the top

    while (!bGameOver)
    {
        // Game Loop Timing


        // User Input


        // Update Game Logic



        // Render the Output

        // Draw the Play Field
        for (int x = 0; x < nFieldWidth; x++)
        {
            for (int y = 0; y < nFieldHeight; y++)
            {
                // offset by 2 so that it has padding in the console
                // Set the characters to write by the values in the pField array
                screen[(y + Y_PADDING) * nScreenWidth + (x + X_PADDING)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
            }
        }

        // Draw the currently falling piece
        for (int px = 0; px < 4; px++)
        {
            for (int py = 0; py < 4; py++)
            {
                // if the tetrimino has occupancy in the current tetromino index
                if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                {
                    // add it to the screen (index + 65) so its 65==A
                    screen[(nCurrentY + py + Y_PADDING) * nScreenWidth + (nCurrentX + px + X_PADDING)] = nCurrentPiece + 65;
                }
            }
        }


        // Display the Frame
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

}