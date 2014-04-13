
//parsira datoteku s porukama
//vraca vektor stringova
vector< string > Pipeline::getMessages(char *file) {
	vector<string> ret;
	string input;
	ifstream in;
	in.open(file, ios::in);

	while(getline(in, input)) {
		ret.push_back(input);
	}

	in.close();
	return ret;
}

// salje poruke
void Pipeline::sendMessage(int pdf, string msg, int type ) {

	msgBuffer *buf = new msgBuffer;
	sprintf(buf->mtext, "%s", msg.c_str());
	buf->mtype = type;
	write( pdf, buf, sizeof(*buf) );

	delete buf;
}

// stvori cjevi
void Pipeline::createPipes() {
	string name = "./pipe";
	for(int i = 0; i < N; i++) {
		pipeNames.push_back( name + int_2_string(i) );
	}

	unlinkPipes(false);

	for(int i = 0 ; i < N ; i++) {
		if ( mknod(pipeNames[i].c_str(), S_IFIFO | 00600, 0) ==-1 )
			error();
	}
}

//obrise cijevi
void Pipeline::unlinkPipes(bool m) {
	for(int i = 0; i < N; i++) {
		if (m) cout << "Brisem cjevovod " << pipeNames[ i ] << endl; 
		unlink( pipeNames[ i ].c_str() );
	}
}

