#include "TreeNode.hpp"
#include "XmlParserTest.h"
#include "XmlParser.h"


XmlParserTest::XmlParserTest() :
  UnitTest("XmlParserTest")
{

}


XmlParserTest::~XmlParserTest()
{

}


void XmlParserTest::SetUp()
{

}


void XmlParserTest::TearDown()
{

}


void XmlParserTest::TestParseXml()
{
  std::string xml_format_nominal =
    "<entry_point>\n"
    " <Table_1>\n"
    "  <Key_1>value1</Key_1>\n"
    "  <Key_2>value2</Key_2>\n"
    " </Table_1>\n"
    " <Table_2>\n"
    "   <SubTable_1>\n"
    "     <Key_3>value3</Key_3>\n"
    "     <Key_4>value4</Key_4>\n"
    "     <!-- This is a comment..-->\n"
    "   </SubTable_1>\n"
    "   <SubTable_2>\n"
    "     <Key_5>value5</Key_5>\n"
    "     <!--Multi\n"
    "         Line\n"
    "         Comment-->\n"
    "   </SubTable_2>\n"
    " </Table_2>\n"
    " <InitTable=\"ABC\">\n"
    "   <InitKey>InitValue</InitKey>\n"
    " </InitTable>\n"
    "</entry_point>"
    ;

  std::ofstream outStream("\\xml_test.xml");
  outStream.write(xml_format_nominal.c_str(), xml_format_nominal.length());
  outStream.close();

  // Verify parsing succeeded
  XmlParser parser;
  TEST_EQ(parser.ParseXml("\\xml_test.xml"), StatusTypes::STATUS_OK);
  
  // Verify no errors were recorded
  uint32_t line = 0;
  std::string syntax_error = "";
  parser.GetParseError(line, syntax_error);
  TEST_EQ(line, 0);
  TEST_EQ(syntax_error.length(), 0);

  // Validate all nodes
  TreeNode<std::string>* node = parser.TopNode;
  TEST_EQ(node->GetKey().compare("entry_point"), 0);
  TEST_EQ(node->GetChildCount(), 3);
  
  node = parser.TopNode->GetChild(0);
  TEST_EQ(node->GetKey().compare("Table_1"), 0);
  node = parser.TopNode->GetChild(0)->GetChild(0);
  TEST_EQ(node->GetKey().compare("Key_1"), 0);
  TEST_EQ(node->GetValue().compare("value1"), 0);
  node = parser.TopNode->GetChild(0)->GetChild(1);
  TEST_EQ(node->GetKey().compare("Key_2"), 0);
  TEST_EQ(node->GetValue().compare("value2"), 0);

  node = parser.TopNode->GetChild(1);
  TEST_EQ(node->GetKey().compare("Table_2"), 0);
  node = parser.TopNode->GetChild(1)->GetChild(0);
  TEST_EQ(node->GetKey().compare("SubTable_1"), 0);
  TEST_EQ(node->GetChildCount(), 2);
  node = parser.TopNode->GetChild(1)->GetChild(0)->GetChild(0);
  TEST_EQ(node->GetKey().compare("Key_3"), 0);
  TEST_EQ(node->GetValue().compare("value3"), 0);
  node = parser.TopNode->GetChild(1)->GetChild(0)->GetChild(1);
  TEST_EQ(node->GetKey().compare("Key_4"), 0);
  TEST_EQ(node->GetValue().compare("value4"), 0);

  node = parser.TopNode->GetChild(1)->GetChild(1);
  TEST_EQ(node->GetKey().compare("SubTable_2"), 0);
  TEST_EQ(node->GetChildCount(), 1);
  node = parser.TopNode->GetChild(1)->GetChild(1)->GetChild(0);
  TEST_EQ(node->GetKey().compare("Key_5"), 0);
  TEST_EQ(node->GetValue().compare("value5"), 0);

  node = parser.TopNode->GetChild(2);
  TEST_EQ(node->GetKey().compare("InitTable"), 0);
  TEST_EQ(node->GetValue().compare("ABC"), 0);
  node = parser.TopNode->GetChild(2)->GetChild(0);
  TEST_EQ(node->GetKey().compare("InitKey"), 0);
  TEST_EQ(node->GetValue().compare("InitValue"), 0);
}


void XmlParserTest::TestParseXmlError()
{
  std::string xml_format_off_nominal =
    "<entry_point>\n"
    " <Table_1>\n"
    "  <Key_1>value1/Key_1>\n" // Error here
    "  <Key_2>value2</Key_2>\n"
    " </Table_1>\n"
    " <Table_2>\n"
    "   <SubTable_1>\n"
    "     <Key_3>value3</Key_3>\n"
    "     <Key_4>value4</Key_4>\n"
    "     <!-- This is a comment..-->\n"
    "   </SubTable_1>\n"
    "   <SubTable_2>\n"
    "     <Key_5>value5</Key_5>\n"
    "     <!--Multi\n"
    "         Line\n"
    "         Comment-->\n"
    "   </SubTable_2>\n"
    " </Table_2>\n"
    " <InitTable>\n"
    "   <InitKey>InitValue</InitKey>\n"
    " </InitTable>\n"
    ;

  std::ofstream outStream("\\xml_test.xml");
  outStream.write(xml_format_off_nominal.c_str(), 
                  xml_format_off_nominal.length());
  outStream.close();

  // Verify parsing succeeded
  XmlParser parser;
  TEST_EQ(parser.ParseXml("\\xml_test.xml"), StatusTypes::XML_PARSE_ERROR);

  // Verify no errors were recorded
  uint32_t line = 0;
  std::string syntax_error = "";
  parser.GetParseError(line, syntax_error);
  TEST_EQ(line, 3);
}

void XmlParserTest::TestGetNode()
{
  std::string xml_format_nominal =
    "<entry_point>\n"
    " <Table_1>\n"
    "  <Key_1>value1</Key_1>\n"
    "  <Key_2>value2</Key_2>\n"
    " </Table_1>\n"
    " <Table_2>\n"
    "   <SubTable_1>\n"
    "     <Key_3>value3</Key_3>\n"
    "     <Key_4>value4</Key_4>\n"
    "   </SubTable_1>\n"
    "   <SubTable_2>\n"
    "     <Key_5>value5</Key_5>\n"
    "   </SubTable_2>\n"
    " </Table_2>\n"
    " <InitTable=\"1\">\n"
    "   <InitKey>InitValue</InitKey>\n"
    " </InitTable>\n"
    "</entry_point>\n"
    ;

  std::ofstream outStream("\\xml_test.xml");
  outStream.write(xml_format_nominal.c_str(), xml_format_nominal.length());
  outStream.close();

  // Verify parsing succeeded
  XmlParser parser;
  TEST_EQ(parser.ParseXml("\\xml_test.xml"), StatusTypes::STATUS_OK);

  // Verify no errors were recorded
  uint32_t line = 0;
  std::string syntax_error = "";
  parser.GetParseError(line, syntax_error);
  TEST_EQ(line, 0);
  TEST_EQ(syntax_error.length(), 0);

  // Test Several sections..
  TreeNode<std::string>* node = parser.GetNode("entry_point");
  TEST_NE(node, nullptr);
  TEST_EQ(node->GetChildCount(), 3);
  node = parser.GetNode("Table_1");
  TEST_NE(node, nullptr);
  TEST_EQ(node->GetChildCount(), 2);
  node = parser.GetNode("InitTable");
  TEST_NE(node, nullptr);
  TEST_EQ(node->GetChildCount(), 1);
  TEST_EQ(node->GetValue().compare("1"), 0);

  // Test invalid section
  node = parser.GetNode("poop");
  TEST_EQ(node, nullptr);
}

TestPkg* XmlParserTest::BuildTestPkg()
{
  INITIALIZE();
  ADD_TEST(XmlParserTest, TestParseXml);
  ADD_TEST(XmlParserTest, TestParseXmlError);
  ADD_TEST(XmlParserTest, TestGetNode);
  FINALIZE();
}