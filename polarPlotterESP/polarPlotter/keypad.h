#ifndef keypad_h
#define keypad_h

#define KPD "KPD:"

class Keypad_Serial {
	unsigned long kTime;
	char ch;
	const char NO_KEY = '\0';

public:
	int procced(String s) {
		if(s) {
			int pos = s.indexOf(KPD);
			if(pos >= 0) {
				char ch = s[pos + strlen(KPD)];
				if(ch == '+')
					ch = '*';
				if(ch == '-')
					ch = '#';
				this->ch = toupper(ch);
				this->kTime = millis();
				return 1;
			}
			if(millis() - this->kTime < 150) {
				return 0;
			}
			this->ch = NO_KEY;
		}
		return 0;
	};

	char getRawKey() {
		/*
		if(Serial.available()) {
			ch = Serial.read();
			if(ch == '+')
				ch = '*';
			if(ch == '-')
				ch = '#';
			ch = toupper(ch);
			kTime = millis();
			return ch;
		}
		if(millis() - kTime < 150) {
			return ch;
		}
		ch = NO_KEY;
		return ch;
		*/
		return this->ch;
	}
    char getKey2() {
    	return getRawKey();
    }
};

#endif
