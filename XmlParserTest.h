#include "UnitTest.h"

class XmlParserTest : public UnitTest
{
public:

  XmlParserTest();
  ~XmlParserTest();

  virtual void SetUp();
  virtual void TearDown();

  void TestParseXml();
  void TestParseXmlError();
  void TestGetNode();

  TestPkg* BuildTestPkg();

private:

};