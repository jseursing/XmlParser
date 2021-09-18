#pragma once
#include "StatusTypes.h"
#include <fstream>

// Forward declarations
template<class Type>
class TreeNode;

/*
 * This class processes input xml files.
 */
class XmlParser
{
public:
    
  StatusTypes::StatusEnum ParseXml(std::string xmlPath);
  TreeNode<std::string>* GetNode(std::string key, 
                                 TreeNode<std::string>* topNode = nullptr);
  std::string GetValue(std::string key, 
                       TreeNode<std::string>* topNode = nullptr);
  void GetParseError(uint32_t& line, std::string& errorStr);
  XmlParser();
  ~XmlParser();

private:
  
  StatusTypes::StatusEnum _ParseXml(std::ifstream& inputStream);
  TreeNode<std::string>* _GetNode(std::string key, 
                                  TreeNode<std::string>*& pNode);
  
  TreeNode<std::string>* TopNode;
  uint32_t ErrorLine;
  std::string SyntaxError;

  // Test Friend declarations
  friend class XmlParserTest;
};