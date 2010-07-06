SET(POCO_DIR "poco")
FIND_PATH(POCO_INCLUDE_DIR Poco/Poco.h PATHS ${POCO_DIR}/include /usr/local/include /usr/include ENV INCLUDE DOC "POCO include directory")

FIND_LIBRARY(POCO_LIBRARY NAMES PocoFoundation PocoFoundationd PATHS ${POCO_DIR}/lib /usr/local/lib /usr/lib ${POCO_INCLUDE_DIR}/../lib ENV LIB)
FIND_LIBRARY(POCO_LIBRARYD NAMES PocoFoundationd PocoFoundation PATHS ${POCO_DIR}/lib /usr/local/lib /usr/lib ${POCO_INCLUDE_DIR}/../lib ENV LIB )

FIND_LIBRARY(POCO_NET_LIBRARY NAMES PocoNet PocoNetd PATHS ${POCO_DIR}/lib /usr/local/lib /usr/lib ${POCO_INCLUDE_DIR}/../lib ENV LIB )
FIND_LIBRARY(POCO_NET_LIBRARYD NAMES PocoNetd PocoNet PATHS ${POCO_DIR}/lib /usr/local/lib /usr/lib ${POCO_INCLUDE_DIR}/../lib ENV LIB )

FIND_LIBRARY(POCO_UTIL_LIBRARY NAMES PocoUtil PocoUtild PATHS ${POCO_DIR}/lib /usr/local/lib /usr/lib ${POCO_INCLUDE_DIR}/../lib ENV LIB )
FIND_LIBRARY(POCO_UTIL_LIBRARYD NAMES PocoUtild PocoUtil PATHS ${POCO_DIR}/lib /usr/local/lib /usr/lib ${POCO_INCLUDE_DIR}/../lib ENV LIB )

FIND_LIBRARY(POCO_XML_LIBRARY NAMES PocoXML PocoXMLd PATHS ${POCO_DIR}/lib /usr/local/lib /usr/lib ${POCO_INCLUDE_DIR}/../lib ENV LIB )
FIND_LIBRARY(POCO_XML_LIBRARYD NAMES PocoXMLd PocoXML PATHS ${POCO_DIR}/lib /usr/local/lib /usr/lib ${POCO_INCLUDE_DIR}/../lib ENV LIB )

IF (POCO_INCLUDE_DIR AND POCO_LIBRARY)
   SET(POCO_FOUND TRUE)
ENDIF (POCO_INCLUDE_DIR AND POCO_LIBRARY)

