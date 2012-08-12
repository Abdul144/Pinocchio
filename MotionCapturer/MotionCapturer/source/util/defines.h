
#ifndef __DEFINES_H__
#define __DEFINES_H__

#ifndef null
	#define null	0
#endif

#define DELETE(pointer)				{ if(pointer != null) { delete pointer; pointer = null; } }
#define DELETE_ARRAY(pointer)		{ if(pointer != null) { delete[] pointer; pointer = null; } }

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char byte;


// getter setter
#define GETTER(varType, funcName, varName)		inline varType get##funcName() const {	return varName;	}
#define SETTER(varType, funcName, varName)		inline void set##funcName(varType value) {	varName = value;	}
#define GETTER_SETTER(varType, funcName, varName)	GETTER(varType, funcName, varName)	SETTER(varType, funcName, varName)

// getter setter const ref
#define GETTER_CONST_REF(varType, funcName, varName)	\
			inline const varType& get##funcName() const {	return varName;	}
#define SETTER_CONST_REF(varType, funcName, varName)	\
			inline void set##funcName(const varType &value) {	varName = value;	}
#define GETTER_SETTER_CONST_REF(varType, funcName, varName)		\
		GETTER_CONST_REF(varType, funcName, varName)	SETTER_CONST_REF(varType, funcName, varName)

// getter setter ref
#define GETTER_REF(varType, funcName, varName)	\
			inline varType& get##funcName() {	return varName;	}
#define GETTER_SETTER_REF(varType, funcName, varName)		\
		GETTER_REF(varType, funcName, varName)	SETTER_CONST_REF(varType, funcName, varName)


#endif















































