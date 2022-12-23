#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <string>

std::mutex m;
std::condition_variable cv;

bool checkWin(std::vector<std::vector<char>>& table, int X, int Y, 
	int N, int winCon) {
	int _X = X, _Y = Y;
	int count = 1;
	while (--_X >= 0)
		if (table[_X][Y] == table[X][Y])
			++count;
		else
			break;
	_X = X;
	while (++_X < N)
		if (table[_X][Y] == table[X][Y])
			++count; 
		else
			break;
	if (count == winCon)
		return true;
	count = 1;
	while (--_Y >= 0)
		if (table[X][_Y] == table[X][Y])
			++count;
		else
			break;
	_Y = Y;
	while (++_Y < N)
		if (table[X][_Y] == table[X][Y])
			++count;
		else
			break;
	if (count == winCon)
		return true;
	_X = X, _Y = Y, count = 1;
	while(--_X >= 0 && --_Y >= 0)
		if (table[_X][_Y] == table[X][Y])
			++count;
		else
			break;
	_X = X, _Y = Y;
	while (++_X < N && ++_Y < N)
		if (table[_X][_Y] == table[X][Y])
			++count;
		else
			break;
	if (count == winCon)
		return true;
	_X = X, _Y = Y, count = 1;
	while (--_X >= 0 && ++_Y < N)
		if (table[_X][_Y] == table[X][Y])
			++count;
		else
			break;
	_X = X, _Y = Y;
	while (++_X < N && --_Y >= 0)
		if (table[_X][_Y] == table[X][Y])
			++count;
		else
			break;
	return (count == winCon) ? true : false;
}

void _move(int id, bool turn[2],
	std::vector<std::vector<char>>& table, int& count, int N,
	int& winner) {
	std::unique_lock<std::mutex> ul(m);
	while (count < 9) {
		cv.wait(ul, [=] {return turn[id - 1]; });
		if (count == 9)
			break;
		int x, y;
		do {
			std::cout << "Player " << id << " move: ";
			std::string PlayerMove;
			std::getline(std::cin, PlayerMove);
			size_t dashPos = PlayerMove.find('-');
			x = stoi(PlayerMove.substr(0, dashPos)) - 1;
			y = static_cast<int>(PlayerMove[dashPos + 1]) - 97;
			if (table[x][y] != 32)
				std::cout << "Move invalid, cell already has been mark!\n";
		} while (table[x][y] != 32);
		table[x][y] = (id == 1) ? 'O' : 'X';
		if (checkWin(table, x, y, N, 3) == true) {
			winner = id;
			count = 9;
			turn[0] = !turn[0];
			turn[1] = !turn[1];
			cv.notify_all();
			return;
		}
		turn[0] = !turn[0];
		turn[1] = !turn[1];
		++count;
		cv.notify_all();
		if (count == 9)
			break;
	}
	winner = 0;
}

int main() {
	int N = 3, count = 0;
	bool turn[2]{ 1 };
	int winner1, winner2;
	std::cout << "\t\t\t\t\t\t\tTICTACTOE\n";
	std::vector<std::vector<char>> table(N, std::vector<char>(N, 32));
	std::thread t1(_move, 1, turn, std::ref(table), std::ref(count), 
		N, std::ref(winner1));
	std::thread t2(_move, 2, turn, std::ref(table), std::ref(count), 
		N, std::ref(winner2));
	cv.notify_all();
	if (t1.joinable())
		t1.join();
	if (t2.joinable())
		t2.join();
	if (winner1 == winner2)
		std::cout << "Tie!!!";
	else
		std::cout << "\t\t\t\t\t\t\tWinner:\n\t\t\t\t\t\t\tPlayer " 
		<< ((winner1 > winner2) ? winner1 : winner2);
	return 0;
}