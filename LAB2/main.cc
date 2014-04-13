#include "main.h"

class Pipeline {
	private:
		int N;
		vector< pid_t > childPID;
		vector< string > pipeNames;
		char *fileLoc;

		void error();
		void createChildren();
		void unlinkPipes(bool);
		void createPipes();
		void startParent();
		void startChild(string, int);
		void forkChildren();
		void waitChildren();

		vector<string> getMessages(char*);
		void sendMessage(int, string, int);

	public:
		Pipeline(int n, char *f) { 
			N = n ; 
			childPID.resize(N, -1);
			fileLoc = f;
		}

		// glavna metoda
		void start();
};

#include "pipe.cc"

void Pipeline::error() {
	cout << "Doslo je do pogreske..." << endl;
	exit(1);
}

// kreira proces roditelj i N djece
// takoder stvara imenovane cjevovode 
void Pipeline::createChildren() {
	forkChildren();

	cout << "Stvoreno " << N << " procesa sluga... " << endl;
	for( int i = 0; i < N; i++) {
		cout << "Proces " << childPID[ i ] << " je sluga " << i;
		cout << " od glavnog procesa " << getppid() << endl;
	}
	cout << "-------------------------" << endl;
}

void Pipeline::forkChildren() {
	for( int i = 0; i < N; i++) {
		childPID[ i ] = fork();

		switch( childPID[ i ] ) {
			case -1: { error(); }
			case 0: {
				startChild(pipeNames[ i ], i);
				exit(1);
			}
		}
	}
}

void Pipeline::waitChildren() {
	int n = N; 
	int status;

	while( n > 0)
	{
		wait(&status);
		n--;
	}
}

void Pipeline::startChild(string pipeName, int r) {
	// otvara cijevovod za citanje 
	int	pfd = open( pipeName.c_str(), O_RDONLY);
	msgBuffer *m;
	ofstream out;
	string name = "data/r"  + int_2_string(r) + ".txt";
	out.open( name.c_str(), ios::out);

	while(1) {
		m = new msgBuffer;
		read( pfd, m, sizeof( *m ));
		if ( m->mtype == -1) { delete m; break; }
		//cout << m->mtext << endl;
		out << m->mtext << endl;
		delete m;
	}	

	cout << "Sluga" << r << " je gotov" << endl;
	out.close();
}

void Pipeline::startParent() {
	srand( time( NULL ) );
	vector< string >msgVec = getMessages(fileLoc);
	vector< int > pfds( N, -2);

	// otvara cjevovode za pisanje
	for( int i = 0; i < N; i++) {
		pfds[ i ] = open(pipeNames[i].c_str(), O_WRONLY);
	}
	
	for(unsigned int i = 0; i < msgVec.size() ; i++) {
		int R = (int)rand() % N;
		cout << "Poslao poruku: '"<< msgVec[ i ] <<  "' procesu sluga" << R << endl;
		sendMessage(pfds[ R ], msgVec[i], R);
	}

	cout << "-------------------------" << endl;

	for(int i = 0; i < N; i++)
		sendMessage(pfds[ i ], "Gotovo", -1);
}

void Pipeline::start() {
	// prvo napravi cjevovode
	createPipes();
	// forkaj djecu
	createChildren();
	// pokrece slanje poruka slugama
	startParent();
	// ceka kraj procesa djece;
	waitChildren();
	// pobrise imenovane cjevovode 
	unlinkPipes(true);
}

int main(int argc, char **argv)
{
	if ( argc != 3 ) {
		cout << "Program je potrebno pokrenuti s ./start [2-5] [dat_poruke]" << endl;
		exit(1);
	}

	Pipeline novi(atoi(argv[ 1 ]), argv[ 2 ]);
	novi.start();

	return 0;
}
