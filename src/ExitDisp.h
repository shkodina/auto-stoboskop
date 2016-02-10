#ifndef EXITDISP_H_2012_10_18
#define EXITDISP_H_2012_10_18

#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));

#define TRUE 0xff
#define FALSE 0x00

struct Tugling{
	const static char ON = 0xff;
	const static char OFF = 0x00;
	const static int ALWAYS = 0xffff;
	const static int NEVER = 0x0000;
	
	char state;
	int ms10_period; 
};

class TuggleAlgorithm{
	public:
	
	const static int MAX_ALG_LEN = 4;
	const static char OK = 0;
	const static char ERR = 1;
	
	TuggleAlgorithm(){
		alglen = 0;
		cur = 0;
	};

	TuggleAlgorithm(Tugling* tug, char tuglen){
		setAlg(tug,tuglen);
	}	

	char setAlg(Tugling* tug, char tuglen){
		if (tuglen > MAX_ALG_LEN) 
			return ERR;
			
		for(int i = 0; i < tuglen; i++)
			alg[i] = tug[i];
			
		alglen = tuglen;
		cur = alglen;

		return 0;
	}
	
	void reset(){ 
		cur = alglen;
	}
	
	Tugling getNext(){
		if (cur++ >= alglen) 
			cur = 0;
			
		return alg[cur];
	}
	
	char setTugling(Tugling tug, unsigned char pos){
		if (pos > alglen)
			return ERR;
		
		alg[pos] = tug;
		return 0;
	}
	
	private:
	
	Tugling alg[MAX_ALG_LEN];
	unsigned char alglen;
	unsigned char cur;
};

class Exit {

	public:
	
	Exit (){};
	
	Exit (char num, TuggleAlgorithm talg){
		number = num;
		alg = talg;
		curtug = alg.getNext();
	}
	
	void reInit(char cnum, TuggleAlgorithm calg){
		number = cnum;
		alg = calg;
		curtug = alg.getNext();
	}
	
	void reset(){
		alg.reset();
		curtug = alg.getNext();
	}
	
	char getState(){
		return curtug.state;
	}
	
	char getNumber(){
		return number;
	}
	
	char isLastTick(){
		if(curtug.ms10_period ==  Tugling::ALWAYS || curtug.ms10_period--) 
			return FALSE;
			
		curtug = alg.getNext();
		return TRUE;
	}
	
	private:
	
	char number;
	TuggleAlgorithm alg;
	Tugling curtug;
	

};

class ProcExit{
	public:

	virtual void tugleExit(char exit_num, char state);
};



class ExitDisp{
	public:

	static const char MAX_EXITS = 32; 
	static const char OK = 0;
	static const char ERR = 1;

	ExitDisp(){
		len = 0;
	}

	ExitDisp(ProcExit * pexit){
		proc_exit = pexit;
		len = 0;
	}

	void setProcExit(ProcExit * pexit){
		proc_exit = pexit;
	}

	char addExit(Exit ext){
		if (len == MAX_EXITS)
			return 0;
		
		exits[len++] = ext;
		return len;	
	}

	char changeExitByPosition(Exit ext, unsigned char pos){
		if (pos >= len)
			return 0;
		
		exits[pos] = ext;
		return len;	
	}

	char changeExitByNumber(Exit ext, char number){
		char changes = 0;
		for(unsigned char i = 0; i < this->len; i++){
			if (exits[i].getNumber() == number){
				exits[i] = ext;
				changes++;
			}	
		}
		return changes;
	}

	void firstCheckExits(){
		for(unsigned char i = 0; i < this->len; i++){
			proc_exit->tugleExit(exits[i].getNumber(), exits[i].getState());
		}
	}
	
	void checkExits(){
		for(unsigned char i = 0; i < this->len; i++){
			if (exits[i].isLastTick()){
				proc_exit->tugleExit(exits[i].getNumber(), exits[i].getState());
			}
		}
	}
	
	private:
	
	Exit exits[MAX_EXITS];
	unsigned char len;
	ProcExit * proc_exit;
	
};

#endif
