#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<map>
#include<set>
#include<queue>
#include<stack>
#include<fstream>
#include<iomanip>

using namespace std;

// 存储翻译表
map<int, string> numToStr;
map<string, int> strToNum;

string symIdToStr(int num) {
	return numToStr[num];
}

int strToSymId(string str) {
	return strToNum[str];
}

class Symbol { // 符号：终结符、非终结符
public:
	enum SymbolType {
		T, NT // terminal or not-terminal
	};
private:
	SymbolType type;
	int id;
public:
	// 构造函数
	Symbol() {
		this->type = SymbolType::T;
		this->id = 2;
	}
	Symbol(SymbolType type, int id) {
		this->type = type;
		this->id = id;
	}

	// 获取私有数据
	SymbolType getType() const {
		return this->type;
	}
	int getId() const {
		return this->id;
	}

	// 重载操作符
	Symbol& operator = (const Symbol& otherSymbol) {
		this->type = otherSymbol.getType();
		this->id = otherSymbol.getId();
		return *this;
	}

	bool operator < (const Symbol& otherSymbol) const {
		return this->id < otherSymbol.id;
	}

	bool operator == (const Symbol& otherSymbol) const {
		return this->id == otherSymbol.id;
	}
	// 输出Symbol
	friend ostream& operator << (ostream& os, const Symbol& word) {
		os << symIdToStr(word.getId());
		return os;
	}
};


// 定义三个特殊符号
const Symbol BEGIN(Symbol::SymbolType::NT, 0); // S'
const Symbol END(Symbol::SymbolType::T, 1); // $
const Symbol EPSILON(Symbol::SymbolType::T, 2); // ε


// 项目(产生式)
class Item {
private:
	Symbol leftSymbol;
	vector<Symbol> rightSymbol;
	Symbol foreSymbol; // 向前看符号
	int dotPosition; // 圆点位置(0 -- rightSymbol.size())
public:
	// 构造函数
	Item() {}
	Item(Symbol leftSymbol, vector<Symbol> rightSymbol, Symbol foreSymbol = END, int dotPosition = 0) {
		this->leftSymbol = leftSymbol;
		this->rightSymbol = rightSymbol;
		this->foreSymbol = foreSymbol;
		this->dotPosition = dotPosition;
	}

	// 获取私有数据
	Symbol getLeftSymbol() const { return leftSymbol; }
	vector<Symbol>& getRightSymbol() { return rightSymbol; }
	const vector<Symbol>& getRightSymbol() const { return rightSymbol; }
	Symbol getForeSymbol() const { return foreSymbol; }
	int getDotPosition() const { return dotPosition; }

	// 重载操作符
	bool operator == (const Item& otherItem) const {
		if (this->leftSymbol == otherItem.getLeftSymbol() &&
			this->foreSymbol == otherItem.getForeSymbol() &&
			this->dotPosition == otherItem.getDotPosition()) {
			int len = rightSymbol.size();
			const vector<Symbol>& temp = otherItem.getRightSymbol();
			if (len != temp.size()) return false;
			else {
				for (int i = 0; i < len; ++i) {
					if (this->rightSymbol[i] == temp[i]) continue;
					else return false;
				}
			}
			return true;
		}
		else return false;
	}

	friend ostream& operator << (ostream& os, Item& item) {
		cout << endl;
		cout << '[' << item.getLeftSymbol() << " --> ";
		int len = item.getRightSymbol().size();
		for (int i = 0; i < len; ++i) {
			if (i == item.getDotPosition()) cout << "·";
			cout << item.getRightSymbol()[i] << " ";
		}
		if (item.getDotPosition() == len) cout << "· ";
		cout << "," << item.getForeSymbol() << ']';
		return os;
	}

	// 当dot在最后表示规约项目
	bool isReductionItem() const {
		return dotPosition == rightSymbol.size();
	}

	// 返回dot后面的第一个符号
	Symbol symbolAfterDot() const {
		if (isReductionItem()) return EPSILON;
		else return rightSymbol[dotPosition];
	}

	// 返回右侧第一个符号
	Symbol firstSymbolOfRight() const {
		return rightSymbol[0];
	}

	// 返回右侧index后的符号串
	vector<Symbol> allSymbolsAfterIndex(int index) const {
		vector<Symbol> temp;
		int len = rightSymbol.size();
		for (int i = index + 1; i < len; ++i)
			temp.push_back(rightSymbol[i]);
		return temp;
	}
};


// 项目集合
class ItemSet {
private:
	int id;
	vector<Item> items;
public:
	// 构造函数
	ItemSet() {}
	ItemSet(int id) {
		this->id = id;
		items = vector<Item>();
	}

	ItemSet(int id, vector<Item>& items) {
		this->id = id;
		this->items.assign(items.begin(), items.end());
	}

	// 获取私有数据
	int getId() const { return id; }
	int getSize() const { return this->items.size(); }
	vector<Item>& getItems() { return items; }
	const vector<Item>& getItems() const { return items; }

	// 重载=
	ItemSet& operator = (const ItemSet& otherItemSet) {
		this->items.assign(otherItemSet.getItems().begin(), otherItemSet.getItems().end());
		return *this;
	}

	// 重载==
	bool operator == (const ItemSet& otherItemSet) const {
		int size = this->getSize();
		if (size != otherItemSet.getSize()) return false;
		else {
			for (int i = 0; i < size; ++i) {
				if (items[i] == otherItemSet.getItems()[i]) continue;
				else return false;
			}
		}
		return true;
	}

	friend ostream& operator << (ostream& os, ItemSet& itemSet) {
		cout << "----------------------" << endl;
		cout << "项目集I" << itemSet.getId() << endl;
		cout << "大小：" << itemSet.getSize() << endl;
		vector<Item>& temp = itemSet.getItems();
		for (int i = 0; i < temp.size(); ++i)
			cout << temp[i];
		cout << endl;
		return os;
	}

	// 修改项目集合id
	void setId(int id) {
		this->id = id;
	}

	// 往项目集合中加入新项目
	void joinItem(Item theItem) {
		items.push_back(theItem);
	}

	// 往项目集合中载入items
	void loadItems(vector<Item>& items) {
		this->items.assign(items.begin(), items.end());
	}
};

// 项目集族(文法)
class ItemSetGroup {
private:
	vector<ItemSet> itemSets;

public:
	ItemSetGroup() {}

	// 获取私有数据
	int getItemSetSize() { return itemSets.size(); }
	vector<ItemSet>& getItemSets() { return itemSets; }

	// 重载操作符
	friend ostream& operator << (ostream& os, ItemSetGroup& itemSet) {
		cout << "ItemSetGrop:" << endl;
		int len = itemSet.getItemSetSize();
		vector<ItemSet>& temp = itemSet.getItemSets();
		for (int i = 0; i < len; ++i) {
			cout << temp[i] << endl;
		}
		return os;
	}

	void loadItemSets(vector<ItemSet>& itemSets) {
		this->itemSets.assign(itemSets.begin(), itemSets.end());
	}
};

// 存储符号X的first()集合
map<Symbol, set<Symbol>> wordFirstSet;

// 存储符号X的follow()集合
map<Symbol, set<Symbol>> wordFollowSet;

// 所有的符号和产生式
set<Symbol> allSymbols;
vector<pair<int, Item>> productions;

// 存储错误信息
vector<pair<int, string>> errorRecord;

// 存储测试文件
vector<pair<Symbol, string>> testStr;
vector<int> columnNum;

// DFA图
vector<set<pair<Symbol, int>>> DFARecord;

// pair.first表示操作类型0->Wrong 1->S 2->R 3->ACK; 
// second表示转移状态或者产生式序号
vector<map<Symbol, pair<int, int>>> action;

// goto
vector<map<Symbol, int>> goTo;

// 判断符号是否为终结符
bool isTerminal(const Symbol theWord) {
	return theWord.getType() == Symbol::SymbolType::T;
}

// 项目是否在项目集中
bool isInItemSet(vector<Item>& items, Item& theItem) {
	int len = items.size();
	for (int i = 0; i < len; ++i)
		if (items[i] == theItem) return true;
	return false;
}

// 项目集是否在项目集族中
int isInItemSetGroup(vector<ItemSet>& itemSets, ItemSet& theItemSet) {
	int len = itemSets.size();
	for (int i = 0; i < len; ++i) {
		if (itemSets[i] == theItemSet) {
			return itemSets[i].getId();
		}
	}
	return -1;
}

// 根据编号在allSymbols中查找symbol
Symbol searchSymbol(int id) {
	set<Symbol>& temp = allSymbols;
	for (auto i = temp.begin(); i != temp.end(); ++i) {
		if ((*i).getId() == id) return (*i);
	}
	return EPSILON;
}

// split分隔函数
void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}


// first(), 求单个符号的可能的第一个终结符
void first() {
	// 对所有终结符来说，它们的first()集合就是它们自己
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		set<Symbol> temp;
		if (isTerminal(*i)) temp.insert(*i);
		wordFirstSet.insert(pair<Symbol, set<Symbol>>(*i, temp));
	}
	bool change = true;
	while (change) {
		change = false;
		// 遍历每一个产生式
		int len = productions.size();
		for (int i = 0; i < len; ++i) {
			Item theItem = productions[i].second;
			set<Symbol>& temp = wordFirstSet[theItem.getLeftSymbol()];
			// 产生式的右边第一个符号为终结符
			Symbol firstRight = theItem.firstSymbolOfRight();
			if (isTerminal(firstRight)) {
				if (temp.find(firstRight) == temp.end()) {
					temp.insert(firstRight);
					change = true;
				}
			}
			// 产生式的右边第一个符号为非终结符
			else {
				// 当这个非终结符能够推导出ε时，需要考虑后面一个终结符
				bool next = true;
				int index = 0, lenOfRight = theItem.getRightSymbol().size();
				while (next && index < lenOfRight) {
					next = false;
					set<Symbol>& joinSet = wordFirstSet[theItem.getRightSymbol()[index]];
					//  当前非终结符joinSet包括ε
					if (joinSet.find(EPSILON) != joinSet.end()) {
						next = true;
						++index;
					}
					for (auto j = joinSet.begin(); j != joinSet.end(); ++j) {
						if (!((*j) == EPSILON) && temp.find(*j) == temp.end()) {
							change = true;
							temp.insert(*j);
						}
					}
				}
				// 此时表示右边产生式的所有符号皆为非终结符且可以推导出ε
				if (index == lenOfRight) {
					temp.insert(EPSILON);
				}
			}
		}
	}
}

// first(), 当前符号串可能的第一个终结符
set<Symbol> first(vector<Symbol> theStr) {
	int len = theStr.size();
	set<Symbol> temp;
	bool next = true;
	int index = 0;
	while (next && index < len) {
		next = false;
		Symbol theWord = theStr[index];
		// 当前字符为终结符，则将此终结符加入temp
		if (isTerminal(theWord)) {
			temp.insert(theWord);
			return temp;
		}
		// 当前字符不是终结符，则根据此字符的first()集合是否含有ε来判断是否需要看下一个字符
		else {
			set<Symbol>& joinSet = wordFirstSet[theStr[index]];
			auto findEpsilon = joinSet.find(EPSILON);
			if (findEpsilon != joinSet.end()) {
				next = true;
				++index;
			}
			for (auto j = joinSet.begin(); j != joinSet.end(); ++j) {
				if (!((*j) == EPSILON) && temp.find(*j) == temp.end()) {
					temp.insert(*j);
				}
			}
		}
	}
	// 当index == len说明此字符串中的每个字符都为非终结符并且可能为ε
	if (index == len) {
		temp.insert(EPSILON);
	}
	return temp;
}

// follow(), 求单个符号的可能的下一个终结符
void follow() {
	// 初始时，每个非终结符的follow()集合都为空
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		if (!isTerminal(*i))
			wordFollowSet[*i] = set<Symbol>();
	}
	// follow(S') = {ε}
	wordFollowSet[BEGIN].insert(END);

	bool change = true;
	int len = productions.size();
	while (change) {
		change = false;
		for (int i = 0; i < len; ++i) {
			// 每个产生式的右侧的符号串
			vector<Symbol>& theRightSymbol = productions[i].second.getRightSymbol();
			int lenOfRight = theRightSymbol.size();
			// 遍历产生式右侧的符号串的每个符号，只考虑非终结符
			for (int j = 0; j < lenOfRight; ++j) {
				Symbol theWord = theRightSymbol[j];
				if (isTerminal(theWord)) { continue; }
				else {
					// 在set中查出当前非终结符已有的follow()集合，并计算出此非终结符后面的符号串的first()集合
					set<Symbol>& theWordFollow = wordFollowSet[theWord];
					set<Symbol> joinSet = first((productions[i].second).allSymbolsAfterIndex(j));
					// 当joinSet中存在一些theWordFollow集合里面没有的终结符，则将它们加入到theWordFollow集合中去
					for (auto k = joinSet.begin(); k != joinSet.end(); ++k) {
						if (theWordFollow.find(*k) == theWordFollow.end() && !((*k) == EPSILON)) {
							change = true;
							theWordFollow.insert(*k);
						}
					}
					// 当joinSet含有为ε（包括当前非终结符后面的字符串的first()集合含有ε或者该字符串本身就为ε）
					auto findEpsilon = joinSet.find(EPSILON);
					if (findEpsilon != joinSet.end()) {
						set<Symbol>& theLeftfollow = wordFollowSet[productions[i].second.getLeftSymbol()];
						for (auto k = theLeftfollow.begin(); k != theLeftfollow.end(); ++k) {
							if (theWordFollow.find(*k) == theWordFollow.end()) {
								change = true;
								theWordFollow.insert(*k);
							}
						}
					}
				}
			}
		}
	}
}


// 求项目集合的闭包
void closure(ItemSet& theItemSet) {
	bool change = true;
	while (change) {
		change = false;
		vector<Item>& allItems = theItemSet.getItems();
		int len = allItems.size();
		// 遍历每个项目
		for (int i = 0; i < len; ++i) {
			Symbol theWordAfterDot = allItems[i].symbolAfterDot();
			// dot后面的是一个非终结符
			if (!isTerminal(theWordAfterDot)) {
				int dotIndex = allItems[i].getDotPosition();
				// 此非终结符后面的字符串，以及它的first()集合
				vector<Symbol> wordsAfter = allItems[i].allSymbolsAfterIndex(dotIndex);
				// 将前看符号加入
				wordsAfter.push_back(allItems[i].getForeSymbol());
				set<Symbol> firstOfWordsAfter = first(wordsAfter);
				// 遍历每个产生式
				int lenOfProduction = productions.size();
				for (int j = 0; j < lenOfProduction; ++j) {
					Item theProduction = productions[j].second;
					// 产生式中左部非终结符和项目dot后的非终结符相同
					if (theProduction.getLeftSymbol() == theWordAfterDot) {
						// 对于每一个b∈FIRST（βa）
						for (auto k = firstOfWordsAfter.begin(); k != firstOfWordsAfter.end(); ++k) {
							// 构造一个新的item[B-->·η,b]
							Item temp(theWordAfterDot, theProduction.getRightSymbol(), (*k), 0);
							// 当此时的closure中没有这个item，将这个item加入closure
							if (!isInItemSet(allItems, temp)) {
								change = true;
								theItemSet.joinItem(temp);
							}
						}
					}
				}
			}
		}
	}
}

// go(I, X) = closure(J)
ItemSet go(ItemSet& I, Symbol theSymbol) {
	vector<Item> temp;
	vector<Item> items = I.getItems();
	int num = I.getId() + 1, len = items.size();
	ItemSet J(num, temp);
	// 遍历I中的所有项目
	for (int i = 0; i < len; ++i) {
		Item theItem = items[i];
		// 当一个项目的dot后的符号为非终结符符且为theSymbol时
		Symbol theSymbolAfterDot = theItem.symbolAfterDot();
		if (!theItem.isReductionItem() && theSymbolAfterDot == theSymbol) {
			Item t(theItem.getLeftSymbol(), theItem.getRightSymbol(),
				theItem.getForeSymbol(), theItem.getDotPosition() + 1);
			J.joinItem(t);
		}
	}
	if (J.getSize() > 0)
		closure(J);
	return J;
}

// 构造项目集族的DFA，已经在itemSetGroup中载入了allSymbols、productions以及BEGIN->S
void DFA(ItemSetGroup& itemSetGroup) {
	ofstream ofTransfer("transfer.txt", ios::out);
	if (!ofTransfer.is_open()) {
		cout << "打开Transfer失败" << endl;
	}
	ofTransfer.clear();

	// 待扩展队列
	queue<ItemSet> waitExpansion;
	// 获取项目集族的各个数据结构
	vector<ItemSet>& itemSets = itemSetGroup.getItemSets();
	ItemSet& I0 = itemSets[0];
	// 对BEGIN->S求closure，得到I0;
	closure(I0);
	cout << "closure后:" << endl;
	cout << I0;
	// 将I0加入待扩展队列
	waitExpansion.push(I0);
	int count = 0;
	while (!waitExpansion.empty()) {
		ItemSet& t = waitExpansion.front();
		set<pair<Symbol, int>> thisTran;
		for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
			Symbol theWord = (*i);
			ItemSet temp = go(t, theWord);
			if (temp.getSize() > 0) {
				int id = isInItemSetGroup(itemSets, temp);
				// 项目集族中存在该项目集
				if (id >= 0) {}
				// 项目集族中不存在该项目，将该项目集加入
				else {
					id = itemSets.size();
					temp.setId(id);
					itemSets.push_back(temp);
					waitExpansion.push(temp);
					cout << temp;
				}
				thisTran.insert(pair<Symbol, int>(theWord, id));
			}
		}

		ofTransfer << "有效项目集I" << count++ << "的转移:" << endl;
		for (auto j = thisTran.begin(); j != thisTran.end(); ++j) {
			ofTransfer << (*j).first << "---" << (*j).second << endl;
		}
		ofTransfer << endl;

		DFARecord.push_back(thisTran);
		waitExpansion.pop();
	}
	ofTransfer.close();
	// 打印DFA
	//cout << "DFA: " << endl;
	//for (int i = 0; i < DFARecord.size(); ++i) {
	//	cout << "有效项目集I" << i << "的转移:" << endl;
	//	set<pair<Symbol, int>>& temp = DFARecord[i];
	//	for (auto j = temp.begin(); j != temp.end(); ++j) {
	//		cout << (*j).first << "---" << (*j).second << endl;
	//	}
	//	cout << endl;
	//}
}

void buildAnalysisTable(ItemSetGroup& itemSetGroup) {
	// 获取项目集族的各个数据结构
	vector<ItemSet>& itemSets = itemSetGroup.getItemSets();

	int itemSetSize = itemSetGroup.getItemSetSize();

	// 更改action、goto表大小
	action.resize(itemSetSize);
	for (int i = 0; i < itemSetSize; ++i) {
		for (auto j = allSymbols.begin(); j != allSymbols.end(); ++j) {
			if (isTerminal(*j)) {
				action[i][*j].first = 0;
				action[i][*j].second = -1;
			}
		}
	}

	goTo.resize(itemSetSize);
	for (int i = 0; i < itemSetSize; ++i) {
		for (auto j = allSymbols.begin(); j != allSymbols.end(); ++j) {
			if (!isTerminal(*j) && !((*j) == BEGIN)) {
				goTo[i][*j] = -1;
			}
		}
	}

	// 遍历项目集族中的每一个项目集
	for (int i = 0; i < itemSetSize; ++i) {
		// 遍历每个项目集中的每个项目
		vector<Item>& tempItems = itemSets[i].getItems();
		for (int j = 0; j < tempItems.size(); ++j) {
			Item& tempItem = tempItems[j];
			// 非规约项目
			if (!tempItem.isReductionItem()) {
				Symbol charAfterDot = tempItem.symbolAfterDot();
				set<pair<Symbol, int>>& tempTran = DFARecord[i];
				// charAfterDot是终结符
				if (isTerminal(charAfterDot)) {
					for (auto k = tempTran.begin(); k != tempTran.end(); ++k) {
						if ((*k).first == charAfterDot) {
							action[i][charAfterDot].first = 1; // 1->S
							action[i][charAfterDot].second = (*k).second;
							break;
						}
					}
				}
				// charAfterDot是非终结符，
				else if (!isTerminal(charAfterDot)) {
					for (auto k = tempTran.begin(); k != tempTran.end(); ++k) {
						if ((*k).first == charAfterDot) {
							goTo[i][charAfterDot] = (*k).second;
							break;
						}
					}
				}
			}
			// 规约项目
			else {
				// 接受项目
				if (tempItem.getLeftSymbol() == BEGIN && tempItem.getForeSymbol() == END)
					action[i][END].first = 3; //3->ACK
				// 规约
				else {
					for (int k = 0; k < productions.size(); ++k) {
						if (productions[k].second.getLeftSymbol() == tempItem.getLeftSymbol() && productions[k].second.getRightSymbol() == tempItem.getRightSymbol()) {
							action[i][tempItem.getForeSymbol()].first = 2; // 2->R
							action[i][tempItem.getForeSymbol()].second = k;
							break;
						}
					}
				}
			}
		}
	}
}

// 对项目集族进行初始化，包括文法、标识符表的读入等
void initItemSetGroup(ItemSetGroup& itemSetGroup) {
	// 载入
	ifstream fTranslateTable("translateTable.txt", ios::in);
	string s1, s2, s3;
	Symbol tempSymbol;
	int num;

	// 载入翻译表和符号表
	if (!fTranslateTable.is_open())
		cout << "无法载入翻译表！！" << endl;
	while (!fTranslateTable.eof()) {
		fTranslateTable >> s1 >> s2 >> s3;
		num = atoi(s1.c_str());
		numToStr.insert({ num, s3 });
		strToNum.insert({ s3, num });
		if (s2 == "T") allSymbols.insert(Symbol(Symbol::SymbolType::T, num));
		else allSymbols.insert(Symbol(Symbol::SymbolType::NT, num));
	}
	fTranslateTable.close();

	// 载入测试文件
	ifstream fStr("testStr.txt", ios::in);
	if (!fStr.is_open()) {
		cout << "无法载入测试文件！！" << endl;
		return;
	}


	while (!fStr.eof()) {
		fStr >> s1 >> s2 >> s3;
		num = atoi(s2.c_str());
		testStr.push_back(make_pair(searchSymbol(strToSymId(s1)), s3));
		columnNum.push_back(num);
	}
	fStr.close();
	testStr.push_back(make_pair(END,"——")); // 在最后加入END
	columnNum.push_back(num);

	// 载入文法(产生式)
	ifstream fGrammer("grammer.txt", ios::in);
	if (!fGrammer.is_open())
		cout << "无法载入文法" << endl;
	// left - right1 right2 right3
	while (getline(fGrammer, s1)) {
		vector<string> tempVec;
		Symbol left;
		vector<Symbol> right;
		Symbol findSymbol;
		int number;
		SplitString(s1, tempVec, " ");
		for (int i = 0; i < tempVec.size(); ++i) {
			if (tempVec[i] == "->") continue;
			else if (i == 0) {
				number = atoi(tempVec[i].c_str());
			}
			else if (i == 1) {
				findSymbol = searchSymbol(strToSymId(tempVec[i]));
				if (!(findSymbol == EPSILON))
					left = findSymbol;
			}
			else {
				findSymbol = searchSymbol(strToSymId(tempVec[i]));
				right.push_back(findSymbol);
			}
		}
		Item tempItem(left, right);
		productions.push_back(make_pair(number,tempItem));
	}
	fGrammer.close();
	cout << "allSymbols:" << endl;
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		cout << (*i) << endl;
	}
	cout << endl;

	cout << "productions:" << endl;
	for (int i = 0; i < productions.size(); ++i) {
		cout << productions[i].second << endl;
	}
	cout << endl;

	first();

	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		set<Symbol> temp = wordFirstSet[*i];
		cout << "符号" << symIdToStr((*i).getId()) << " 的first（）：";
		for (auto j = temp.begin(); j != temp.end(); ++j) {
			cout << symIdToStr((*j).getId()) << ' ';
		}
		cout << endl;
	}

	cout << endl;
	follow();
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		set<Symbol> temp = wordFollowSet[*i];
		cout << "符号" << symIdToStr((*i).getId()) << " 的follow（）：";
		for (auto j = temp.begin(); j != temp.end(); ++j) {
			cout << symIdToStr((*j).getId()) << ' ';
		}
		cout << endl;
	}

	// 构造ItemSetGroup
	Symbol s = searchSymbol(3);
	vector<Symbol> sr; sr.push_back(s);
	Item p1(BEGIN, sr);
	vector<Item> tt;
	tt.push_back(p1);
	ItemSet I0(0, tt);

	vector<ItemSet> itemSets;
	itemSets.push_back(I0);
	itemSetGroup.loadItemSets(itemSets);
}

// 打印action、goto表
void printTable() {
	cout << "action:\t";
	for (int i = 0; i < action[0].size(); ++i) cout << "\t";
	cout << " goto:" << endl << "\t";
	// 先打印非终结符
	for (auto j = action[0].begin(); j != action[0].end(); ++j) {
		cout << symIdToStr((*j).first.getId()) << "\t";
	}
	cout << "|\t";
	for (auto j = goTo[0].begin(); j != goTo[0].end(); ++j) {
		cout << symIdToStr((*j).first.getId()) << "\t";
	}
	cout << endl;
	for (int i = 0; i < action.size(); ++i) {
		cout << "I" << i << "\t";
		map<Symbol, pair<int, int>>& tempMap1 = action[i];
		for (auto j = tempMap1.begin(); j != tempMap1.end(); ++j) {
			if ((*j).second.first == 1) { // 1->S
				cout << "S" << (*j).second.second << "\t";
			}
			else if ((*j).second.first == 2) { // 2->R
				cout << "R" << (*j).second.second << "\t";
			}
			else if ((*j).second.first == 3) { // 3->ACK
				cout << "ACK" << "\t";
			}
			else { // 0->Wrong
				cout << "\t";
			}
		}
		cout << "|\t";
		map<Symbol, int>& tempMap2 = goTo[i];
		for (auto j = tempMap2.begin(); j != tempMap2.end(); ++j) {
			if ((*j).second == -1) { // 1->S
				cout << "\t";
			}
			else { // 0->Wrong
				cout << (*j).second << "\t";
			}
		}
		cout << endl;
	}
}

// 存储action、goto表到文件中
void storeTable() {
	//vector<map<Symbol, pair<int, int>>> action;
	//vector<map<Symbol, int>> goTo;


	ofstream ofAction("action.txt", ios::out);
	if (!ofAction.is_open()) {
		cout << "打开table失败" << endl;
	}
	ofAction.clear();
	ofAction << action.size() << endl;
	for (int i = 0; i < action.size(); ++i) {
		map<Symbol, pair<int, int>>& tempMap = action[i];
		Symbol tempSymbol;
		for (auto j = tempMap.begin(); j != tempMap.end(); ++j) {
			tempSymbol = (*j).first;
			if (tempSymbol.getType() == Symbol::SymbolType::NT) ofAction << "NT\t";
			else ofAction << "T\t";
			ofAction << tempSymbol.getId() << "\t" << (*j).second.first << "\t" << (*j).second.second << "\t";
		}
		ofAction << "#" << endl;
	}
	ofAction.close();

	ofstream ofGoto("goto.txt", ios::out);
	if (!ofGoto.is_open()) {
		cout << "打开table失败" << endl;
	}
	ofGoto.clear();
	ofGoto << goTo.size() << endl;
	for (int i = 0; i < goTo.size(); ++i) {
		map<Symbol, int>& tempMap = goTo[i];
		Symbol tempSymbol;
		for (auto j = tempMap.begin(); j != tempMap.end(); ++j) {
			tempSymbol = (*j).first;
			if (tempSymbol.getType() == Symbol::SymbolType::NT) ofGoto << "NT\t";
			else ofGoto << "T\t";
			ofGoto << tempSymbol.getId() << "\t" << (*j).second << "\t";
		}
		ofGoto << "#" << endl;
	}
	ofGoto.close();
}

// 从文件中读入action、goto表
void loadTable() {
	//vector<map<Symbol, pair<int, int>>> action;
	//vector<map<Symbol, int>> goTo;

	ifstream ifAction("action.txt", ios::in);
	if (!ifAction.is_open()) {
		cout << "打开Action table失败" << endl;
	}
	string s1, s2, s3, s4;
	int size;
	ifAction >> s1;
	size = atoi(s1.c_str());

	for (int i = 0; i < size; ++i) {
		map<Symbol, pair<int, int>> tempMap;
		ifAction >> s1;
		while (s1 != "#" && !ifAction.eof()) {
			ifAction >> s2 >> s3 >> s4;

			Symbol tempSymbol(Symbol::SymbolType::T, atoi(s2.c_str()));
			tempMap.insert({ tempSymbol, pair<int, int>(atoi(s3.c_str()), atoi(s4.c_str())) });

			ifAction >> s1;
		}
		action.push_back(tempMap);
	}
	ifAction.close();

	ifstream ifGto("goto.txt", ios::in);
	if (!ifGto.is_open()) {
		cout << "打开goTo table失败" << endl;
	}
	ifGto >> s1;
	size = atoi(s1.c_str());

	for (int i = 0; i < size; ++i) {
		map<Symbol, int> tempMap;
		ifGto >> s1;
		while (s1 != "#" && !ifGto.eof()) {
			ifGto >> s2 >> s3;

			Symbol tempSymbol(Symbol::SymbolType::NT, atoi(s2.c_str()));
			tempMap.insert({ tempSymbol, atoi(s3.c_str()) });

			ifGto >> s1;
		}
		goTo.push_back(tempMap);
	}
	ifGto.close();
}

class TreeNode {
public:
	Symbol parent;
	int childNum;
	vector<TreeNode>* child;

	TreeNode() {
		this->parent = BEGIN;
		this->childNum = 0;
		this->child = new vector<TreeNode>;
	}

	TreeNode(Symbol symbol) {
		this->parent = symbol;
		this->childNum = 0;
		this->child = new vector<TreeNode>;
	}

	TreeNode& operator = (TreeNode& otherNode) {
		this->parent = otherNode.parent;
		this->childNum = otherNode.childNum;
		(*(this->child)).assign(otherNode.child->begin(), otherNode.child->end());
		return *this;
	}

	void joinVec(vector<TreeNode>& child) {
		(*(this->child)).assign(child.begin(), child.end());
		this->childNum = child.size();
	}
};

stack<TreeNode> treeStk;
TreeNode root(BEGIN);

void printTree(TreeNode& node, int num) {
	int t;
	if (isTerminal(node.parent)) cout << node.parent;
	else cout << node.parent.getId();
	for (int i = 0; i < node.childNum; ++i) {
		t = num;
		if (i == 0) cout << "\t";
		if (i != 0) {
			while (t-- >= 0) {
				cout << "\t";
			}
		}
		printTree((*(node.child))[i], num + 1);
	}
	if (node.childNum == 0) cout << "\n";
}

struct Record {
	string name;
	string type;
	int value;
	int address;
	int dimension;
	int declareColumn;
	vector<int> citeColumn;
	Record(string name, string type, int column) {
		this->name = name;
		this->type = type;
		this->declareColumn = column;
		this->citeColumn = vector<int>();
	}
};


struct SymbolTable {
	vector<Record> symStk;
	stack<int> indexStk;
	SymbolTable() {
		symStk = vector<Record>();
		indexStk = stack<int>();
		indexStk.push(0);
	}

	void buildNewTable() {
		indexStk.push(symStk.size());
	}

	void removeTable() {
		int num = indexStk.top();
		indexStk.pop();

		while (symStk.size() > num) {
			symStk.pop_back();
		}
	}

	int searchRecord(string name) {
		for (int i = symStk.size() - 1; i >= 0; --i) {
			if (symStk[i].name == name)
				return i;
		}
		return -1;
	}

	void joinTable(string name, string type, int column) {
		if (searchRecord(name) == -1) {
			Record temp(name, type, column);
			symStk.push_back(temp);
		}
		else {
			cout << "重定义！！" << endl;
		}
	}
};

struct Attribute {
	string Type;
	int value;
	int numberInTable;
	Attribute() {
		Type = "";
		value = 0;
		numberInTable = -1;
	}
};

struct semanticTreeNode {
	string element;
	string classify;
	semanticTreeNode* left;
	semanticTreeNode* right;
	semanticTreeNode(string element, string classify) {
		this->element = element;
		this->classify = classify;
		this->left = nullptr;
		this->right = nullptr;
	}

};

void printSemanticTree(semanticTreeNode* node, int num) {
	if (node == nullptr) { 
		cout << endl; 
		return;
	}
	
	cout << node->element << "\t";
	printSemanticTree(node->left, num+1);

	int t = num;
	while (t-- >= 0) cout << "\t";
	printSemanticTree(node->right, num + 1);

	return;
}


SymbolTable mySymbolTable;

void process() {
	stack<int> stateStk;
	stack<pair<Symbol, Attribute>> symbolStk;
	stack<semanticTreeNode*> semanticTreeStk;
	stateStk.push(0);
	int ip = 0;
	int state;
	Symbol tempSymbol;
	string describe;
	int column;
	string type = "";

	do {
		//vector<map<Symbol, pair<int, int>>> action;
		//vector<map<Symbol, int>> goTo;
		// 构建符号表
		tempSymbol = testStr[ip].first;
		describe = testStr[ip].second;
		column = columnNum[ip];

		state = stateStk.top();
		if (action[state][tempSymbol].first == 1) { // 1->S

			// 创建新的符号表或删去符号表
			if (symIdToStr(tempSymbol.getId()) == "{") {
				cout << "\n{\n" << endl;
				mySymbolTable.buildNewTable();
			}
			else if (symIdToStr(tempSymbol.getId()) == "}") {
				cout << "\n}\n" << endl;
				mySymbolTable.removeTable();
			}


			cout << "移进字符：\t" << tempSymbol << endl;
			//cout << "去到状态：\t" << action[state][tempSymbol].second << endl;
			stateStk.push(action[state][tempSymbol].second);
			Attribute tempAttribute;
			// 如果移入id,并且符号表中尚不存在
			if (symIdToStr(tempSymbol.getId()) == "id") {
				int t = mySymbolTable.searchRecord(describe);
				if (t == -1) {
					if (type == "" && symIdToStr((testStr[ip-1].first).getId()) != "struct") { // 没有specifier，即非声明语句
						cout << "变量未定义!!\n\n" << endl;
						errorRecord.push_back({ columnNum[ip], "变量未定义!" });
						break;
						
					}
					else { // 从type获取类型，加入符号表
						cout << "符号表加入" << describe << ",类型为: " << type << "\n\n" << endl;
						tempAttribute.Type = type;
						mySymbolTable.joinTable(describe, type, column);
					}
				}
				else { // 符号表中存在
					if(type == "" && symIdToStr((testStr[ip - 1].first).getId()) != "struct") {
						tempAttribute.Type = mySymbolTable.symStk[t].type;
						tempAttribute.value = mySymbolTable.symStk[t].value;
						tempAttribute.numberInTable = t;
					}
					else {
						cout << "变量重定义!!\n\n" << endl;
						errorRecord.push_back({ columnNum[ip], "变量重定义!" });
					}
				}
			}
			// 如果移入num
			else if (symIdToStr(tempSymbol.getId()) == "num") {
				tempAttribute.value = atoi(describe.c_str());
				tempAttribute.Type = "int";
				
			}

			symbolStk.push(make_pair(tempSymbol, tempAttribute));
			treeStk.push(TreeNode(tempSymbol));
			++ip;
		}
		else if (action[state][tempSymbol].first == 2) { // 2->R
			int number = productions[action[state][tempSymbol].second].first;
			Item& production = productions[action[state][tempSymbol].second].second;
			cout << "规约：\t" << production << endl;

			vector<TreeNode> childVec = vector<TreeNode>();
			vector<pair<Symbol, Attribute>> computeVec = vector<pair<Symbol, Attribute>>();
			for (int i = 0; i < production.getRightSymbol().size(); ++i) {
				stateStk.pop();
				computeVec.push_back(symbolStk.top());
				symbolStk.pop();
				childVec.push_back(treeStk.top());
				treeStk.pop();
			}
			state = stateStk.top();
			//cout << "转移到状态:" << goTo[state][production.getLeftSymbol()] << endl;


			TreeNode newNode = TreeNode(production.getLeftSymbol());
			newNode.joinVec(childVec);
			//cout << "加入新节点:" << production.getLeftSymbol() << endl;

			treeStk.push(newNode);
			stateStk.push(goTo[state][production.getLeftSymbol()]);
			Attribute tempAttribute;
			// 如果是specifier，则有type
			if (number == 10 || number == 11 || number == 12) { //  TYPE -> int || TYPE -> float || TYPE -> bool
				cout << "TYPE, type   =   " << symIdToStr(computeVec[0].first.getId()) << endl;
				tempAttribute.Type = symIdToStr(computeVec[0].first.getId());
			}
			else if (number == 9) { // Specifier -> TYPE
				type = computeVec[0].second.Type;
				cout << "specifier, type   =   " << type << endl;
				tempAttribute.Type = type;
			} 
			else if (number == 4 || number == 5 || number == 6) {
				cout << "一条完整的声明语句！！！\n"  << endl;
				type = "";
			}
			else if (number == 29) { // 29 Stmt -> Exp ;
				if (!semanticTreeStk.empty()) {
					semanticTreeNode* temp = semanticTreeStk.top(); semanticTreeStk.pop();
					cout << "语法树：：\n\n" << endl;
					printSemanticTree(temp, 0);
					cout << "\n\n" << endl;
				}
			}
			else if (number == 58) { // Exp -> id
				int t = computeVec[0].second.numberInTable;
				//	mySymbolTable.searchRecord(testStr[ip-1].second);
				//if (t == -1)
				//		cout << "变量未定义!!\t\t" << describe << endl;
				//else { // 符号表中存在
				tempAttribute.numberInTable = t;
				tempAttribute.Type = mySymbolTable.symStk[t].type;
				tempAttribute.value = mySymbolTable.symStk[t].value;
				semanticTreeStk.push(new semanticTreeNode(mySymbolTable.symStk[t].name, "id"));
				//}
			}
			else if (number == 59) { // Exp -> num
				tempAttribute.Type = computeVec[0].second.Type;
				tempAttribute.value = computeVec[0].second.value;
				semanticTreeStk.push(new semanticTreeNode(to_string(computeVec[0].second.value), "num"));
			}
			else if (number == 43) { // 43 Exp -> Exp = Exp
				// 首先判断类型
				Attribute temp2 = computeVec[0].second;
				Attribute temp1 = computeVec[2].second;
				tempAttribute.Type = "int";
				if (temp1.Type != temp2.Type) {
					cout << "类型不匹配！！\n" << endl;
					errorRecord.push_back({ columnNum[ip], "类型不匹配！" });
					tempAttribute.value = 0;
				}
				else {
					cout << "赋值语句：" << temp2.value << endl;
					mySymbolTable.symStk[temp1.numberInTable].value = temp2.value;
					tempAttribute.value = 1;
				}

				semanticTreeNode* Node1, *Node2;
				Node2 = semanticTreeStk.top(); semanticTreeStk.pop();
				Node1 = semanticTreeStk.top(); semanticTreeStk.pop();

				semanticTreeNode* newNode = new semanticTreeNode("=", "op");
				newNode->left = Node1;
				newNode->right = Node2;
				semanticTreeStk.push(newNode);
			}
			else if (number == 45) { // 45 Exp -> Exp + Exp
				Attribute temp2 = computeVec[0].second;
				Attribute temp1 = computeVec[2].second;
				tempAttribute.Type = "int";
				if (temp1.Type != temp2.Type || !(temp1.Type == "int" || temp1.Type == "float")) {
					cout << "类型不匹配！！\n" << endl;
					errorRecord.push_back({ columnNum[ip], "类型不匹配！" });
				}
				else {
					cout << "算术语句：" << temp1.value << " + " << temp2.value << endl;
					tempAttribute.value = temp1.value + temp2.value;
					semanticTreeNode* Node1, *Node2;
					Node2 = semanticTreeStk.top(); semanticTreeStk.pop();
					Node1 = semanticTreeStk.top(); semanticTreeStk.pop();
					
					semanticTreeNode* newNode = new semanticTreeNode("+", "op");
					newNode->left = Node1;
					newNode->right = Node2;
					semanticTreeStk.push(newNode);
				}
			}
			else if (number == 46) { // 46 Exp -> Exp - Exp
				Attribute temp2 = computeVec[0].second;
				Attribute temp1 = computeVec[2].second;
				tempAttribute.Type = "int";
				if (temp1.Type != temp2.Type || !(temp1.Type == "int" || temp1.Type == "float")) {
					cout << "类型不匹配！！\n" << endl;
					errorRecord.push_back({ columnNum[ip], "类型不匹配！" });
				}
				else {
					cout << "算术语句：" << temp1.value << " - " << temp2.value << endl;
					tempAttribute.value = temp1.value - temp2.value;

					semanticTreeNode* Node1, *Node2;
					Node2 = semanticTreeStk.top(); semanticTreeStk.pop();
					Node1 = semanticTreeStk.top(); semanticTreeStk.pop();

					semanticTreeNode* newNode = new semanticTreeNode("-", "op");
					newNode->left = Node1;
					newNode->right = Node2;
					semanticTreeStk.push(newNode);
				}
			}
			else if (number == 47) { // 47 Exp -> Exp * Exp
				Attribute temp2 = computeVec[0].second;
				Attribute temp1 = computeVec[2].second;
				tempAttribute.Type = "int";
				if (temp1.Type != temp2.Type || !(temp1.Type == "int" || temp1.Type == "float")) {
					cout << "类型不匹配！！\n" << endl;
					errorRecord.push_back({ columnNum[ip], "类型不匹配！" });
				}
				else {
					cout << "算术语句：" << temp1.value << " * " << temp2.value << endl;
					tempAttribute.value = temp1.value * temp2.value;

					semanticTreeNode* Node1, *Node2;
					Node2 = semanticTreeStk.top(); semanticTreeStk.pop();
					Node1 = semanticTreeStk.top(); semanticTreeStk.pop();

					semanticTreeNode* newNode = new semanticTreeNode("*", "op");
					newNode->left = Node1;
					newNode->right = Node2;
					semanticTreeStk.push(newNode);
				}
			}
			else if (number == 48) { // 48 Exp -> Exp / Exp
				Attribute temp2 = computeVec[0].second;
				Attribute temp1 = computeVec[2].second;
				tempAttribute.Type = "int";
				if (temp1.Type != temp2.Type || !(temp1.Type == "int" || temp1.Type == "float")) {
					cout << "类型不匹配！！\n" << endl;
					errorRecord.push_back({ columnNum[ip], "类型不匹配！" });
				}
				else {
					cout << "算术语句：" << temp1.value << " / " << temp2.value << endl;
					tempAttribute.value = temp1.value / temp2.value;

					semanticTreeNode* Node1, *Node2;
					Node2 = semanticTreeStk.top(); semanticTreeStk.pop();
					Node1 = semanticTreeStk.top(); semanticTreeStk.pop();

					semanticTreeNode* newNode = new semanticTreeNode("/", "op");
					newNode->left = Node1;
					newNode->right = Node2;
					semanticTreeStk.push(newNode);
				}
			}
			else if (number == 51) { // 51 Exp -> ( Exp )
				tempAttribute.numberInTable = computeVec[1].second.numberInTable;
				tempAttribute.Type = computeVec[1].second.Type;
				tempAttribute.value = computeVec[1].second.value;
			}
			else if (number == 21 || number == 22) {// 21 FunDec -> id ( VarList )  22 FunDec->id()			
				type = "";
			}
			
			symbolStk.push(make_pair(production.getLeftSymbol(), tempAttribute));
		}
		else if (action[state][tempSymbol].first == 3) { // 3->ACK
			vector<TreeNode> childVec = vector<TreeNode>();
			if (!treeStk.empty())
				childVec.push_back(treeStk.top());
			root.joinVec(childVec);
			cout << "ACC!\n" << endl;

			printTree(root, 0);
			return;
		}
		else { // 0->Wrong
			// 读入空的情况，假错
			if (action[state][EPSILON].first == 1) {
				cout << "移进字符：\t" << EPSILON << endl;
				//cout << "去到状态：\t" << action[state][EPSILON].second << endl;
				stateStk.push(action[state][EPSILON].second);
				symbolStk.push(make_pair(EPSILON, Attribute()));
				treeStk.push(TreeNode(EPSILON));
			}// 读入空也错，真错
			else {
				cout << "Error!\n" << endl;
				cout << tempSymbol << endl;

				pair<Symbol, Attribute> temp1, temp2;
				temp2 = symbolStk.top(); symbolStk.pop();
				temp1 = symbolStk.top(); symbolStk.pop();

				if((symIdToStr(testStr[ip].first.getId()) == "id" || symIdToStr(testStr[ip].first.getId()) == "num") &&
				 symIdToStr(testStr[ip-1].first.getId()) == "id" || symIdToStr(testStr[ip-1].first.getId()) == "num"))
					errorRecord.push_back({ columnNum[ip], "丢失运算符!" });
				else if (symIdToStr(testStr[ip].first.getId()) == ")" && (symIdToStr(testStr[ip - 1].first.getId()) == "+" ||
					symIdToStr(testStr[ip - 1].first.getId()) == "-" || symIdToStr(testStr[ip - 1].first.getId()) == "*" ||
					symIdToStr(testStr[ip - 1].first.getId()) == "/"))
					errorRecord.push_back({ columnNum[ip], "丢失操作数!" });
				else errorRecord.push_back({ columnNum[ip], "语义分析错误!" });

				symbolStk.push(temp1);
				symbolStk.push(temp2);
				// 首先退出当前状态栈顶元素
				cout << "退出状态栈：" << stateStk.top() << endl;
				stateStk.pop();
				int popState;
				Symbol popSymbol;
				while (!symbolStk.empty() && !stateStk.empty() && stateStk.top() != 0) {
					popState = stateStk.top();
					popSymbol = symbolStk.top().first;
					// 一般回退到的非终结符是表达式、语句、函数或过程等
					if (!isTerminal(popSymbol) && (popSymbol.getId() != 23) &&
						goTo[popState][popSymbol] != -1)
						break;
					stateStk.pop();
					symbolStk.pop();
					treeStk.pop();
				}
				stateStk.push(goTo[popState][popSymbol]);
				int flag = 1;

				while (ip < testStr.size()) {
					if (wordFollowSet[popSymbol].find(testStr[ip].first) != wordFollowSet[popSymbol].end()) {
						flag = 0;
						break;
					}
					cout << "丢弃字符" << testStr[ip].first << endl;
					++ip;
				}
				if (ip >= testStr.size()) return;
				if (flag) {
					cout << "死循环" << endl;
					return;
				}
			}
		}
	} while (1);
}
void printError() {
	cout << "错误统计：" << endl;
	cout << "错误总数：" << errorRecord.size() << endl;
	cout << "错误行号\t" << "错误类型\t" << endl;
	for (int i = 0; i < errorRecord.size(); ++i)
		cout << errorRecord[i].first << "\t" << errorRecord[i].second << endl;
}


void main() {
	// 有效项目集族
	ItemSetGroup itemSetGroup;
	initItemSetGroup(itemSetGroup);

	//DFA(itemSetGroup);

	//cout << itemSetGroup << endl;

	//buildAnalysisTable(itemSetGroup);

	//storeTable();
	loadTable();

	//printTable();



	process();

	printError();



	//vector<map<Symbol, pair<int, int>>> action;
	//vector<map<Symbol, int>> goTo;
	// 打印action和goto表

}