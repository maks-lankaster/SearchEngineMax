#ifndef SEARCHENGINEMAX_EXCEPTIONS_H
#define SEARCHENGINEMAX_EXCEPTIONS_H

class ConfigFileIsMissingException: public std::exception {
public:
    const char* what() const noexcept override {
        return "Config file is missing";
    }
};

class ConfigFileIsEmptyException: public std::exception {
public:
    const char * what() const noexcept override {
        return "Config file is empty";
    }
};

class FileNotFoundException: public std::exception {
public:
    const char* what() const noexcept override {
        return "File not found";
    }
};

class IncorrectVersionException: public std::exception {
public:
    const char* what() const noexcept override {
        return "config.json has incorrect file version";
    }
};

class FileNotWritableException: public std::exception {
public:
    const char* what() const noexcept override {
        return "Can't write to answers.json";
    }
};

#endif //SEARCHENGINEMAX_EXCEPTIONS_H
