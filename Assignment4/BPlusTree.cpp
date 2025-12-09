#include "BPlusTree.h"
#include <iostream>
#include <algorithm>  // for std::sort

using namespace std;

// Simple structure for index entries: (largest key in child block, child's RBN)
struct IndexEntry 
{
    std::string key;     // largest key in the child block
    int childRBN;        // RBN of child block (Relative Block Number)
};

// Simple node for our in-memory index (right now we only use one: the root)
struct BPlusTreeNode 
{
    bool isLeaf;                      // true if leaf node (not used yet)
    std::vector<IndexEntry> entries;  // index entries
    int RBN;                          // id for this node (for debugging)
};

// Global in-memory index: root node is at index rootNodeID (usually 0)
static std::vector<BPlusTreeNode> indexNodes;
static int rootNodeID = -1;           // -1 means "no index built yet"


// Constructor: properly initialize members in declaration order
BPlusTree::BPlusTree(const std::string& fname, int blkSize)
    : rootRBN(-1), blockSize(blkSize), filename(fname), seqSet(fname)
{
    // Nothing else to do; seqSet initialized with file
}

// Build a static, one-level B+ tree index from the sequence set blocks
void BPlusTree::BuildStaticIndex()
{
    // Clear any previously built index
    indexNodes.clear();
    rootNodeID = -1;

    const std::vector<Block>& blocks = seqSet.getBlocks();

    if (blocks.empty())
    {
        cout << "BuildStaticIndex: no blocks to index.\n";
        rootRBN = -1;
        return;
    }

    // Create a single root index node
    BPlusTreeNode root;
    root.isLeaf = false;
    root.RBN = 0;   // arbitrary id for debugging
    root.entries.clear();

    // For each non-empty leaf block, create one index entry:
    //   key      = largest key in that block
    //   childRBN = that block's RBN
    for (const Block& block : blocks)
    {
        if (block.GetRecordCount() == 0)
        {
            continue; // skip completely empty blocks
        }

        IndexEntry entry;
        entry.key      = block.getHighestKey(); // assumes Block::getHighestKey() exists
        entry.childRBN = block.GetRBN();

        root.entries.push_back(entry);
    }

    if (root.entries.empty())
    {
        cout << "BuildStaticIndex: all blocks are empty.\n";
        rootRBN = -1;
        return;
    }

    // Ensure root entries are sorted by key (usually already true if blocks are in key order)
    std::sort(root.entries.begin(), root.entries.end(),
              [](const IndexEntry& a, const IndexEntry& b)
              {
                  return a.key < b.key;
              });

    indexNodes.push_back(root);
    rootNodeID = 0;   // root is at index 0 of indexNodes

    // For now, rootRBN just mirrors the in-memory root node id.
    // Later, when we move the index into file blocks, this will change.
    rootRBN = 0;

    cout << "BuildStaticIndex: built root with "
         << root.entries.size() << " children.\n";
}



// Insert a record into the B+ Tree (currently delegates to seqSet)
void BPlusTree::Insert(const std::string& record)
{
    seqSet.Insert(record);
}

// Search for a record by key using the B+ tree root index.
// Falls back to linear scan if no index has been built yet.
// Search for a record by key using the B+ tree root index.
// Falls back to linear scan if no index has been built yet.
bool BPlusTree::Search(const std::string& key, std::string& outRecord)
{
    // If we don't have an index yet, fall back to the original linear search.
    if (rootNodeID < 0 || indexNodes.empty())
    {
        return seqSet.Search(key, outRecord);
    }

    // 1. Use the root index node to decide which leaf block to search.
    const BPlusTreeNode& root = indexNodes[rootNodeID];
    int targetRBN = -1;

    // Choose the first child whose key >= search key.
    // If none, use the last child.
    for (const IndexEntry& entry : root.entries)
    {
        if (key <= entry.key)
        {
            targetRBN = entry.childRBN;
            break;
        }
    }

    if (targetRBN == -1 && !root.entries.empty())
    {
        // key is larger than all entry keys; go to the rightmost child
        targetRBN = root.entries.back().childRBN;
    }

    if (targetRBN == -1)
    {
        // No suitable child found (should not happen if index is consistent)
        return false;
    }

    // 2. Grab all blocks and find the one with that RBN.
    const std::vector<Block>& blocks = seqSet.getBlocks();

    for (const Block& block : blocks)
    {
        if (block.GetRBN() != targetRBN)
        {
            continue;
        }

        // 3. Search only within the chosen block's records.
        //    Key = substring before the first comma.
        const std::vector<std::string>& records = block.getRecords();
        for (const std::string& rec : records)
        {
            std::string recordKey = rec.substr(0, rec.find(','));

            if (recordKey == key)
            {
                outRecord = rec;
                return true;
            }
            else if (recordKey > key)
            {
                // Records in the block are sorted by key, so no need to continue.
                return false;
            }
        }

        // We reached the end of the block without finding the key.
        return false;
    }

    // No block with targetRBN was found; index and sequence set are inconsistent.
    return false;
}


// Delete by key
bool BPlusTree::Delete(const std::string& key)
{
    return seqSet.Delete(key);
}

// Print summary of tree
void BPlusTree::PrintSummary() const
{
    cout << "BPlusTree Summary:" << endl;
    cout << "Root RBN: " << rootRBN << endl;
    cout << "Block size: " << blockSize << endl;
    seqSet.PrintSummary();
}

void BPlusTree::DumpTree(std::ostream& out) const
{
    out << "Dumping B+ Tree Structure:\n";

    if (rootNodeID < 0 || indexNodes.empty())
    {
        out << "No index built.\n";
        return;
    }

    const BPlusTreeNode& root = indexNodes[rootNodeID];

    out << "Root Node (indexNodes[" << rootNodeID
        << "], RBN " << root.RBN << "):\n";

    for (const IndexEntry& entry : root.entries)
    {
        out << "  Key: " << entry.key
            << ", Child RBN: " << entry.childRBN << '\n';
    }
}
