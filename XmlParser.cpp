#include "TreeNode.hpp" 
#include "XmlParser.h"


/////////////////////////////////////////////////////////////////////////////////////////
//
// Function: ParseXml
// Notes:    None
//
/////////////////////////////////////////////////////////////////////////////////////////
StatusTypes::StatusEnum XmlParser::ParseXml(std::string xmlPath)
{
  // Open the file
  std::ifstream inputStream(xmlPath.c_str());
  if (false == inputStream.is_open())
  {
    return StatusTypes::XML_INVALID_FILE;
  }

  // Clear all errors
  ErrorLine = 0;
  SyntaxError = "";

  // Clear TopNode
  if (nullptr != TopNode)
  {
    delete TopNode;
    TopNode = nullptr;
  }

  // Parse the file
  StatusTypes::StatusEnum status = _ParseXml(inputStream);

  // Close the file
  inputStream.close();

  return status;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Function: GetSectionNode
// Notes:    None
//
/////////////////////////////////////////////////////////////////////////////////////////
TreeNode<std::string>* 
XmlParser::GetNode(std::string key, TreeNode<std::string>* topNode)
{
  return _GetNode(key, nullptr == TopNode ? topNode : TopNode);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Function: GetValue
// Notes:    None
//
/////////////////////////////////////////////////////////////////////////////////////////
std::string 
XmlParser::GetValue(std::string key, TreeNode<std::string>* topNode)
{
  std::string value = "";

  TreeNode<std::string>* node = _GetNode(key, nullptr == TopNode ? topNode : TopNode);
  if (node != nullptr)
  {
    value = node->GetValue();
  }

  return value;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Function: GetParseError
// Notes:    None
//
/////////////////////////////////////////////////////////////////////////////////////////
void XmlParser::GetParseError(uint32_t& line, std::string& errorStr)
{
  line = ErrorLine;
  errorStr = SyntaxError;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Function: _ParseXml
// Notes:    None
//
/////////////////////////////////////////////////////////////////////////////////////////
StatusTypes::StatusEnum XmlParser::_ParseXml(std::ifstream& inputStream)
{
  // Define current and previous Parent node
  TreeNode<std::string>* parentNode = nullptr;

  // Process the file line by line
  std::string currLine = "";
  bool commentActive = false;
  for (int curr_line = 1; std::getline(inputStream, currLine); ++curr_line)
  {
    // If a comment is active, skip this line until it terminates.
    if (true == commentActive)
    {
      if (std::string::npos != currLine.find("-->"))
      {
        commentActive = false;
      }

      continue;
    }

    // Find an opening brace for an entry..
    int startKeyPos = currLine.find("<");
    if (std::string::npos == startKeyPos)
    {
      ErrorLine = curr_line;
      SyntaxError = std::string("Syntax Error: ") + currLine;
      break;
    }

    // Check to see if this is the prolog.
    if (std::string::npos != currLine.find("<?xml"))
    {
      continue;
    }

    // Check to see if this is the start of a comment..
    if (std::string::npos != currLine.find("<!--", startKeyPos))
    {
      commentActive = true;
      
      // Check to see if this is a single-line comment.
      if (std::string::npos != currLine.find("-->"))
      {
        commentActive = false;
      }

      continue;
    }

    // Find closing brace for this entry..
    int startKeyEndPos = currLine.find(">", startKeyPos);
    if (std::string::npos == startKeyEndPos)
    {
      ErrorLine = curr_line;
      SyntaxError = std::string("Syntax Error: ") + currLine;
      break;
    }

    // Retrieve key string
    std::string key = currLine.substr(startKeyPos + 1, 
                                      startKeyEndPos - startKeyPos - 1);

    // Check for a forward slash denoting end of parent.
    if ('/' == key[0])
    {
      // There must be a parent if we reach this point.
      if (nullptr == parentNode)
      {
        ErrorLine = curr_line;
        SyntaxError = std::string("Syntax Error: ") + currLine;
        break;
      }

      // Update the key, and validate the previous parent holds the key.
      key = key.substr(1);
      if (0 != key.compare(parentNode->GetKey().c_str()))
      {
        ErrorLine = curr_line;
        SyntaxError = std::string("Syntax Error: ") + currLine;
        break;
      }

      // Set parent to previous parent
      parentNode = parentNode->GetParentNode();
      continue;
    }

    // Check to see if this is a section
    int endKeyPos = currLine.find("<", startKeyEndPos);
    int endKeyEndPos = 0;
    if (std::string::npos == endKeyPos)
    {
      // There shouldn't be a closing brace here..
      endKeyEndPos = currLine.find(">", startKeyEndPos + 1);
      if (std::string::npos != endKeyEndPos)
      {
        ErrorLine = curr_line;
        SyntaxError = std::string("Syntax Error: ") + currLine;
        break;
      }

      // This is a section, if we dont have a parent node, set this as such,
      // otherwise add this node to the parent and update parent. Before adding
      // the node, check to see if the key is paired with a value, add it as such.
      std::string keyVal = "";

      int keyValPos = key.find("=\"");
      if (std::string::npos != keyValPos)
      {
        keyVal = key.substr(keyValPos + 2);

        // Strip the end quotes.. otherwise there is an error
        int endKeyVal = keyVal.find("\"");
        if (std::string::npos == endKeyVal)
        {
          ErrorLine = curr_line;
          SyntaxError = std::string("Syntax Error: ") + currLine;
          break;
        }

        keyVal = keyVal.substr(0, endKeyVal);
        key = key.substr(0, keyValPos);
      }

      if (nullptr == TopNode)
      {
        TopNode = new TreeNode<std::string>(key, keyVal);
        parentNode = TopNode;
      }
      else
      {
        TreeNode<std::string>* pParentNode = parentNode;
        parentNode = new TreeNode<std::string>(key, keyVal, pParentNode);
        pParentNode->AddChild(parentNode);
      }

      continue; // Skip the rest of the line.
    }

    // At this point, this is a key value pair, find the end of the key
    // for validation.
    endKeyEndPos = currLine.find(">", endKeyPos);
    if (std::string::npos == endKeyPos)
    {
      ErrorLine = curr_line;
      SyntaxError = std::string("Syntax Error: ") + currLine;
      break;
    }

    // Retrieve key end string
    std::string keyEnd = currLine.substr(endKeyPos + 1, 
                                          endKeyEndPos - endKeyPos - 1);

    // Check for a forward slash denoting end of key.
    if ('/' != keyEnd[0])
    {
      ErrorLine = curr_line;
      SyntaxError = std::string("Syntax Error: ") + currLine;
      break;
    }

    // Validate the keys match..
    keyEnd = keyEnd.substr(1);
    if (0 != keyEnd.compare(key.c_str()))
    {
      ErrorLine = curr_line;
      SyntaxError = std::string("Syntax Error: ") + currLine;
      break;
    }

    // Validate parent node is valid.
    if (nullptr == parentNode)
    {
      ErrorLine = curr_line;
      SyntaxError = std::string("Syntax Error: ") + currLine;
      break;
    }

    // Add child node to parent.
    std::string value = currLine.substr(startKeyEndPos + 1, 
                                        endKeyPos - startKeyEndPos - 1);
    parentNode->AddChild(new TreeNode<std::string>(key, value, parentNode));
  }

  // Verify the first section was terminated if there is no error.
  if ((0 == ErrorLine) &&
      (nullptr != parentNode))
  {
    ErrorLine = -1;
    SyntaxError = std::string("Syntax Error: First section not terminated");
  }
  
  return (0 != ErrorLine ? StatusTypes::XML_PARSE_ERROR : StatusTypes::STATUS_OK);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Function: _GetNode
// Notes:    None
//
/////////////////////////////////////////////////////////////////////////////////////////
TreeNode<std::string>* 
XmlParser::_GetNode(std::string key, TreeNode<std::string>*& pNode)
{
  // End case (recursive)
  if (nullptr == pNode)
  {
    return nullptr;
  }

  // Check this node for the specified key..
  if (0 == pNode->GetKey().compare(key))
  {
    return pNode;
  }
  
  // Recursively invoke this function for each child..
  TreeNode<std::string>* node = nullptr;
  for (size_t i = 0; i < pNode->GetChildCount(); ++i)
  {
    TreeNode<std::string>* rNode = pNode->GetChild(i);

    node = _GetNode(key, rNode);
    if (nullptr != node)
    {
      break;
    }
  }

  return node;
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Function: XmlParser
// Notes:    None
//
/////////////////////////////////////////////////////////////////////////////////////////
XmlParser::XmlParser() :
  TopNode(nullptr),
  ErrorLine(0)
{

}

/////////////////////////////////////////////////////////////////////////////////////////
//
// Function: ~XmlParser
// Notes:    None
//
/////////////////////////////////////////////////////////////////////////////////////////
XmlParser::~XmlParser()
{
  if (nullptr != TopNode)
  {
    delete TopNode;
  }
}