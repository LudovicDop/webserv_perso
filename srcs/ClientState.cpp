#include "Client.hpp"

/*Constructeur*/
ClientState::ClientState()
	:	_state(WAITING_HEADERS),
		_hasContentLength(false),
		_contentLength(0), 
		_currentContentLength(0),
		_bufferHeader(""),
		_bufferBody(""),
		_fileOpened(false)
	{}

ClientState::~ClientState() {
	if (this->_fileOpened && !_tmpFilePath.empty()) {
		this->_uploadFile.close();
		if (std::remove(_tmpFilePath.c_str()))
			std::cerr << "~ClientState: Failed to delete " << _tmpFilePath << std::endl;
	}
}

/*Getters*/
const e_ClientState&	ClientState::getClientState() const
{
	return (_state);
}

 std::string&	ClientState::getBufferHeader() 
{
	return (_bufferHeader);
}

 std::string& ClientState::getBufferBody() 
{
	return (_bufferBody);
}

unsigned long long ClientState::getContentLength() const { return (_contentLength); }

unsigned long long ClientState::getCurrentContentLength() const{ return (_currentContentLength); }

std::ofstream &ClientState::getUploadFile() { return this->_uploadFile; }

const std::string &ClientState::getTmpFilePath() const {return this->_tmpFilePath; }

const bool &ClientState::getIsFileOpened() const { return this->_fileOpened; }

const std::string &ClientState::getResponse() const { return this->_response; }

const bool &ClientState::getHasContentLength() const {return this->_hasContentLength; }


/*Setters*/
void	ClientState::setClientState(const e_ClientState &state)
{
	_state = state;
}

void	ClientState::setBufferHeader(const std::string &bufferHeader)
{
	_bufferHeader = bufferHeader;
}

void	ClientState::setBufferBody(const std::string& bufferBody)
{
	_bufferBody = bufferBody;
}

void	ClientState::setCurrentContentLength(unsigned long long currentContentLength)
{
	_currentContentLength = currentContentLength;
}

void	ClientState::setContentLength(unsigned long long contentLength)
{
	_contentLength = contentLength;
}

void	ClientState::setTmpFilePath(const std::string &filepath) {
	this->_tmpFilePath = filepath;
}

void	ClientState::setIsFileOpened(const bool isOpened) {
	this->_fileOpened = isOpened;
}

void	ClientState::setResponse(const std::string &response) {
	this->_response = response;
}

void	ClientState::setHasContentLength(const bool hasContentLength) {
	this->_hasContentLength = hasContentLength;
}

void ClientState::printInfo() const
{
	std::cout << "\033[1mCurrent state: \033[0m";
	switch (_state)
	{
		case 0:
			std::cout << "\033[36m🔵 WAITING_HEADERS\033[0m" << std::endl;
			break;
		case 1:
			std::cout << "\033[33m🟡 WAITING_BODY\033[0m" << std::endl;
			break;
		case 2:
			std::cout << "\033[34m⚙️  PROCESSING\033[0m" << std::endl;
			break;
		case 3:
			std::cout << "\033[32m✅ RESPONDING\033[0m" << std::endl;
			break;
		default:
			std::cout << "\033[31m❌ UNKNOWN STATE\033[0m" << std::endl;
	}
}

bool	ClientState::init_upload(int fd) {
	std::stringstream ss;
	ss << "/tmp/" << fd; 
	this->_tmpFilePath =  ss.str();

	this->_uploadFile.open(this->_tmpFilePath.c_str(), std::ios::binary | std::ios::trunc);
	static char buffer[1024 * 1024]; 
    _uploadFile.rdbuf()->pubsetbuf(buffer, sizeof(buffer));
	this->_fileOpened = true;
	return this->_uploadFile.is_open();
}

void ClientState::append_data(const char* data, ssize_t size) {
	_uploadFile.write(data, size);
	_currentContentLength = std::min(_currentContentLength + size, _contentLength);
}

void ClientState::finalize_upload() {
	if (_uploadFile.is_open()) {
		_uploadFile.close();
	}
	_state = PROCESSING;
}
bool ClientState::is_upload_complete() const {
	return _currentContentLength >= _contentLength;
}

bool ClientState::is_file_initialized() const {
	return _uploadFile.is_open();
}

void	ClientState::appendToBufferHeader(const char *data, ssize_t length) {
	this->_bufferHeader.append(data, length);
}
void	ClientState::appendToBufferBody(const char *data, ssize_t length) {
	this->_bufferBody.append(data, length);
}

void ClientState::reset() {
    _state = WAITING_HEADERS;
	std::string empty;
    _bufferHeader.swap(empty);
    _bufferBody.swap(empty);
    _response.swap(empty);
	_hasContentLength = false;
    _contentLength = 0;
    _currentContentLength = 0;
    
    if (_fileOpened) {
        _uploadFile.close();
		if (std::remove(_tmpFilePath.c_str()))
			std::cerr << "Failed to delete " << _tmpFilePath << std::endl;
        _fileOpened = false;
		_tmpFilePath.clear();
    }
}