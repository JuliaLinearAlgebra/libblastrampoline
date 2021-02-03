// Preprocessor annoyances
#define CONCAT2(x,y)    x##y
#define CONCAT(x,y)     CONCAT2(x, y)
#define CNAMEADDR(name) CONCAT(CNAME(name),_addr)