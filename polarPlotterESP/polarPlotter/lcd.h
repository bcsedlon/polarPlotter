#ifndef lcd_h
#define lcd_h

#include "libraries/OLED/SSD1306.h"

class LCD {
	SSD1306 *display;
	//static const int rows = 2;
	//static const int cols = 16;
	//char buffer[rows][cols];
	char *buffer;
	int rows, cols;
	int r, c;
	int rHeight = 10;//16;
	//int cWidth = 5;//8;

public:
	LCD(SSD1306 *display, const int cols, const int rows) {
		this->buffer = new char[rows * cols];
		this->display = display;
		this->c = 0;
		this->r = 0;
		this->cols = cols;
		this->rows = rows;

	}
	void noBacklight() {
		this->display->invertDisplay();
	}
	void backlight() {
		this->display->normalDisplay();
	}
	void clear() {
		for(int i = 0; i < this->rows; i++)
			for(int j = 0; j < this->cols; j++)
				//this->buffer[i][j] = ' ';
				*(this->buffer + (i * this->cols) + j) = ' ';
		this->r = 0;
		this->c = 0;

		this->display->clear();
	}

	void print(int i) {
		print(String(i));
	}
	void print(unsigned int i) {
		print(String(i));
	}
	void print(long i) {
		print(String(i));
	}
	void print(unsigned long i) {
		print(String(i));
	}
	void print(float i) {
		print(String(i));
	}
	void print(double i) {
		print(String(i));
	}



	void print(char ch) {
		*(this->buffer + (this->r * this->cols) + this->c) = ch;
		this->c++;
		this->c = _min(this->c, this->cols - 1);
		//this->print(String(ch));
	}

	void print(String s) {
		//strncpy(this->buffer[this->c], s.c_str(), this->cols - this->c);
		strncpy((char*)(this->buffer + this->cols * this->r + this->c), s.c_str(), this->cols - this->c);
		this->c = _min(this->c + s.length(), this->cols - 1);
		//this->draw();
	}
	void setCursor(int c, int r) {
		this->c = c;
		this->r = r;
	}
	void write(char ch) {
		if(this->c + 1 >= this->cols)
			return;
		//this->buffer[this->r][this->c] = ch;
		*(this->buffer + this->r * this->cols + this->c) = ch;
		this->c++;
		/*
		Serial.print("\nch: ");
		Serial.println(ch);
		Serial.print("LCD: ");
		Serial.print("r: ");
		Serial.print(this->r);
		Serial.print(" c: ");
		Serial.print(this->c);
		Serial.print(" s: ");
		Serial.print(this->buffer[this->r]);
		*/
	}

	void begin(int c, int r) {
		//display->init();
		display->setFont(ArialMT_Plain_10);
		//display->setFont(ArialMT_Plain_16);
		display->setTextAlignment(TEXT_ALIGN_LEFT);
		//display->drawString(0, 0, "SSD1306");
		display->display();
	}
	void draw() {
		this->display->clear();

		for(int i = 0; i < this->rows; i++)
			for(int j = 0; j < this->cols; j++)
				if(*(this->buffer + (i * this->cols) + j) == 0)
					*(this->buffer + (i * this->cols) + j) = ' ';

		for(int r = 0; r < this->rows; r++) {
			//*(this->buffer + (r * this->cols) + this->cols) = '\0';
			//this->display->drawString(0, r * this->rHeight, (char*)(this->buffer + r * this->cols));
			String s = String(this->buffer + r * this->cols);

			Serial.print("LCD");
			Serial.print(r);
			Serial.print(':');
			Serial.println(s.substring(0, this->cols));

			//Serial.println(s.substring(0, this->cols));
			this->display->drawString(0, r * this->rHeight, s.substring(0, this->cols));
		}
		this->display->display();
	}
};

#endif
