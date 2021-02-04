// Preprocessor annoyances
#define CONCAT2(x,y)    x##y
#define CONCAT(x,y)     CONCAT2(x, y)
#define CNAMEADDR(name) CONCAT(CNAME(name),_addr)
#define STR_(x)         #x
#define STR(x)          STR_(x)
#define I(x)            x