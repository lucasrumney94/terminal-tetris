#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

#include <stdio.h>
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
    //std::cout << "Hello World!" << std::endl;

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
    int nCurrentPiece = 3;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2; // center the falling piece
    int nCurrentY = 0; // at the top

    // keypressed Right/Left/Down/Z
    bool bKey[4];
    bool bRotateHold = false;

    // Game Speed
    int nSpeed = 20;
    int nSpeedCounter = 0;
    bool bForcePieceDown = false;
    int nPieceCount = 0;
    int nScore = 0;

    std::vector<int> vLines;

    while (!bGameOver)
    {
        // Game Loop Timing
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        nSpeedCounter++;
        bForcePieceDown = (nSpeedCounter == nSpeed);


        // User Input
        for (int key = 0; key < 4; key++)
        {
            // get if key is pressed \x27 is hex for Right, ... Left, Down, & Z
            bKey[key] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[key]))) != 0;
        }

        // Update Game Logic
        // Left Pressed
        if (bKey[1])
        {
            // Check if it fits to the left
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
            {
                // if it fits the set it
                nCurrentX = nCurrentX - 1;
            }
        }
        // Right Pressed
        if (bKey[0])
        {
            // Check if it fits to the right
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
            {
                // if it fits the set it
                nCurrentX = nCurrentX + 1;
            }
        }
        // Down Pressed
        if (bKey[2])
        {
            // Check if it fits below
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
            {
                // if it fits the set it
                nCurrentY = nCurrentY + 1;
            }
        }

        // Z Pressed
        if (bKey[3])
        {
            // if rotate isn't already held
            if (!bRotateHold)
            {
                // Check if it can rotate
                if (DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY))
                {
                    // if it fits the set the rotation
                    nCurrentRotation = nCurrentRotation + 1;
                }
            }
            // we are holding z
            bRotateHold = true;
        }
        else
        {
            // not holding z
            bRotateHold = false;
        }

        if (bForcePieceDown)
        {
            // Check if it fits below
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
            {
                // if it fits the set it
                nCurrentY = nCurrentY + 1;
            }
            // the piece can't fit, so we need to lock it
            else
            {
                // Lock the current piece into the field
                for (int px = 0; px < 4; px++)
                {
                    for (int py = 0; py < 4; py++)
                    {
                        // if the tetrimino has occupancy in the current tetromino index
                        if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                        {
                            pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
                        }
                    }
                }

                nPieceCount++;
                if (nPieceCount % 10 == 0)
                {
                    if (nSpeed >= 10) nSpeed--;
                }

                // Check have we created any full horizontal lines
                // only check where the last tetromino was placed
                for (int py = 0; py < 4; py++)
                {
                    if (nCurrentY + py < nFieldHeight - 1)
                    {
                        bool bLine = true;
                        for (int px = 1; px < nFieldWidth-1; px++)
                        {
                            bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
                        }
                        if (bLine)
                        {
                            // start at px = 1 to exclude the boundaries
                            for (int px = 1; px < nFieldWidth - 1; px++)
                            {
                                // set the Field to '=' on the line
                                pField[(nCurrentY + py) * nFieldWidth + px] = 8;
                            }

                            vLines.push_back(nCurrentY + py);
                        }
                    }
                }

                // for every piece award 25 points
                nScore += 25;
                // add a (2^n)*100 for n lines cleared using bitshift
                if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;


                // Choose the next Piece
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = std::rand() % 7;

                // if next piece doesn't fit in , game over
                bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }

            // reset speed counter
            nSpeedCounter = 0;
        }

        

        // Render the Output

        // Draw the Play Field
        for (int x = 0; x < nFieldWidth; x++)
        {
            for (int y = 0; y < nFieldHeight; y++)
            {
                // offset by 2 so that it has padding in the console
                // Set the characters to write by the values in the pField array
                screen[(y + Y_PADDING) * nScreenWidth + (x + X_PADDING)] = L" ▓▓▓▓▓▓▓■█"[pField[y * nFieldWidth + x]];
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
                    screen[(nCurrentY + py + Y_PADDING) * nScreenWidth + (nCurrentX + px + X_PADDING)] = L" ░░░░░░░=#"[nCurrentPiece];//nCurrentPiece + 65;
                }
            }
        }

        // Draw the Score
        swprintf_s(&screen[2 * nScreenWidth + nFieldHeight + X_PADDING], 16, L"SCORE: %8d", nScore);

        // if we did match a line (vLines is not empty)
        if (!vLines.empty())
        {
            // Display the frame
            WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth* nScreenHeight, { 0,0 }, & dwBytesWritten);
            // Delay for a bit
            std::this_thread::sleep_for(std::chrono::milliseconds(300));

            for (auto &v : vLines)
            {
                for (int px = 1; px < nFieldWidth - 1; px++)
                {
                    for (int py = v; py > 0; py--)
                    {
                        pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];   
                    }
                    pField[px] = EMPTY_ID;
                }
            }
            // clear the lines that needed to be cleared
            vLines.clear();
        }

        // Display the Frame
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    // Game loop exited, game lost
    CloseHandle(hConsole);
    std::cout << "Game Over! Final Score: " << nScore << std::endl;

    std::cin.get();
    
}