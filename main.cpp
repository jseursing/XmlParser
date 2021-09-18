#include "XmlParserTest.h"
#include "TestController.h"

int main(int argc, char* argv[])
{
  TestController controller;
  controller.AddUnitTest(new XmlParserTest());
  controller.ExecuteAllUnitTests();
}