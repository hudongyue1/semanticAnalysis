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

// �洢�����
map<int, string> numToStr;
map<string, int> strToNum;

string symIdToStr(int num) {
	return numToStr[num];
}

int strToSymId(string str) {
	return strToNum[str];
}

class Symbol { // ���ţ��ս�������ս��
public:
	enum SymbolType {
		T, NT // terminal or not-terminal
	};
private:
	SymbolType type;
	int id;
public:
	// ���캯��
	Symbol() {
		this->type = SymbolType::T;
		this->id = 2;
	}
	Symbol(SymbolType type, int id) {
		this->type = type;
		this->id = id;
	}

	// ��ȡ˽������
	SymbolType getType() const {
		return this->type;
	}
	int getId() const {
		return this->id;
	}

	// ���ز�����
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
	// ���Symbol
	friend ostream& operator << (ostream& os, const Symbol& word) {
		os << symIdToStr(word.getId());
		return os;
	}
};


// ���������������
const Symbol BEGIN(Symbol::SymbolType::NT, 0); // S'
const Symbol END(Symbol::SymbolType::T, 1); // $
const Symbol EPSILON(Symbol::SymbolType::T, 2); // ��


// ��Ŀ(����ʽ)
class Item {
private:
	Symbol leftSymbol;
	vector<Symbol> rightSymbol;
	Symbol foreSymbol; // ��ǰ������
	int dotPosition; // Բ��λ��(0 -- rightSymbol.size())
public:
	// ���캯��
	Item() {}
	Item(Symbol leftSymbol, vector<Symbol> rightSymbol, Symbol foreSymbol = END, int dotPosition = 0) {
		this->leftSymbol = leftSymbol;
		this->rightSymbol = rightSymbol;
		this->foreSymbol = foreSymbol;
		this->dotPosition = dotPosition;
	}

	// ��ȡ˽������
	Symbol getLeftSymbol() const { return leftSymbol; }
	vector<Symbol>& getRightSymbol() { return rightSymbol; }
	const vector<Symbol>& getRightSymbol() const { return rightSymbol; }
	Symbol getForeSymbol() const { return foreSymbol; }
	int getDotPosition() const { return dotPosition; }

	// ���ز�����
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
			if (i == item.getDotPosition()) cout << "��";
			cout << item.getRightSymbol()[i] << " ";
		}
		if (item.getDotPosition() == len) cout << "�� ";
		cout << "," << item.getForeSymbol() << ']';
		return os;
	}

	// ��dot������ʾ��Լ��Ŀ
	bool isReductionItem() const {
		return dotPosition == rightSymbol.size();
	}

	// ����dot����ĵ�һ������
	Symbol symbolAfterDot() const {
		if (isReductionItem()) return EPSILON;
		else return rightSymbol[dotPosition];
	}

	// �����Ҳ��һ������
	Symbol firstSymbolOfRight() const {
		return rightSymbol[0];
	}

	// �����Ҳ�index��ķ��Ŵ�
	vector<Symbol> allSymbolsAfterIndex(int index) const {
		vector<Symbol> temp;
		int len = rightSymbol.size();
		for (int i = index + 1; i < len; ++i)
			temp.push_back(rightSymbol[i]);
		return temp;
	}
};


// ��Ŀ����
class ItemSet {
private:
	int id;
	vector<Item> items;
public:
	// ���캯��
	ItemSet() {}
	ItemSet(int id) {
		this->id = id;
		items = vector<Item>();
	}

	ItemSet(int id, vector<Item>& items) {
		this->id = id;
		this->items.assign(items.begin(), items.end());
	}

	// ��ȡ˽������
	int getId() const { return id; }
	int getSize() const { return this->items.size(); }
	vector<Item>& getItems() { return items; }
	const vector<Item>& getItems() const { return items; }

	// ����=
	ItemSet& operator = (const ItemSet& otherItemSet) {
		this->items.assign(otherItemSet.getItems().begin(), otherItemSet.getItems().end());
		return *this;
	}

	// ����==
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
		cout << "��Ŀ��I" << itemSet.getId() << endl;
		cout << "��С��" << itemSet.getSize() << endl;
		vector<Item>& temp = itemSet.getItems();
		for (int i = 0; i < temp.size(); ++i)
			cout << temp[i];
		cout << endl;
		return os;
	}

	// �޸���Ŀ����id
	void setId(int id) {
		this->id = id;
	}

	// ����Ŀ�����м�������Ŀ
	void joinItem(Item theItem) {
		items.push_back(theItem);
	}

	// ����Ŀ����������items
	void loadItems(vector<Item>& items) {
		this->items.assign(items.begin(), items.end());
	}
};

// ��Ŀ����(�ķ�)
class ItemSetGroup {
private:
	vector<ItemSet> itemSets;

public:
	ItemSetGroup() {}

	// ��ȡ˽������
	int getItemSetSize() { return itemSets.size(); }
	vector<ItemSet>& getItemSets() { return itemSets; }

	// ���ز�����
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

// �洢����X��first()����
map<Symbol, set<Symbol>> wordFirstSet;

// �洢����X��follow()����
map<Symbol, set<Symbol>> wordFollowSet;

// ���еķ��źͲ���ʽ
set<Symbol> allSymbols;
vector<pair<int, Item>> productions;

// �洢������Ϣ
vector<pair<int, Symbol>> errorRecord;

// �洢�����ļ�
vector<pair<Symbol, string>> testStr;
vector<int> columnNum;

// DFAͼ
vector<set<pair<Symbol, int>>> DFARecord;

// pair.first��ʾ��������0->Wrong 1->S 2->R 3->ACK; 
// second��ʾת��״̬���߲���ʽ���
vector<map<Symbol, pair<int, int>>> action;

// goto
vector<map<Symbol, int>> goTo;

// �жϷ����Ƿ�Ϊ�ս��
bool isTerminal(const Symbol theWord) {
	return theWord.getType() == Symbol::SymbolType::T;
}

// ��Ŀ�Ƿ�����Ŀ����
bool isInItemSet(vector<Item>& items, Item& theItem) {
	int len = items.size();
	for (int i = 0; i < len; ++i)
		if (items[i] == theItem) return true;
	return false;
}

// ��Ŀ���Ƿ�����Ŀ������
int isInItemSetGroup(vector<ItemSet>& itemSets, ItemSet& theItemSet) {
	int len = itemSets.size();
	for (int i = 0; i < len; ++i) {
		if (itemSets[i] == theItemSet) {
			return itemSets[i].getId();
		}
	}
	return -1;
}

// ���ݱ����allSymbols�в���symbol
Symbol searchSymbol(int id) {
	set<Symbol>& temp = allSymbols;
	for (auto i = temp.begin(); i != temp.end(); ++i) {
		if ((*i).getId() == id) return (*i);
	}
	return EPSILON;
}

// split�ָ�����
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


// first(), �󵥸����ŵĿ��ܵĵ�һ���ս��
void first() {
	// �������ս����˵�����ǵ�first()���Ͼ��������Լ�
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		set<Symbol> temp;
		if (isTerminal(*i)) temp.insert(*i);
		wordFirstSet.insert(pair<Symbol, set<Symbol>>(*i, temp));
	}
	bool change = true;
	while (change) {
		change = false;
		// ����ÿһ������ʽ
		int len = productions.size();
		for (int i = 0; i < len; ++i) {
			Item theItem = productions[i].second;
			set<Symbol>& temp = wordFirstSet[theItem.getLeftSymbol()];
			// ����ʽ���ұߵ�һ������Ϊ�ս��
			Symbol firstRight = theItem.firstSymbolOfRight();
			if (isTerminal(firstRight)) {
				if (temp.find(firstRight) == temp.end()) {
					temp.insert(firstRight);
					change = true;
				}
			}
			// ����ʽ���ұߵ�һ������Ϊ���ս��
			else {
				// ��������ս���ܹ��Ƶ�����ʱ����Ҫ���Ǻ���һ���ս��
				bool next = true;
				int index = 0, lenOfRight = theItem.getRightSymbol().size();
				while (next && index < lenOfRight) {
					next = false;
					set<Symbol>& joinSet = wordFirstSet[theItem.getRightSymbol()[index]];
					//  ��ǰ���ս��joinSet������
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
				// ��ʱ��ʾ�ұ߲���ʽ�����з��Ž�Ϊ���ս���ҿ����Ƶ�����
				if (index == lenOfRight) {
					temp.insert(EPSILON);
				}
			}
		}
	}
}

// first(), ��ǰ���Ŵ����ܵĵ�һ���ս��
set<Symbol> first(vector<Symbol> theStr) {
	int len = theStr.size();
	set<Symbol> temp;
	bool next = true;
	int index = 0;
	while (next && index < len) {
		next = false;
		Symbol theWord = theStr[index];
		// ��ǰ�ַ�Ϊ�ս�����򽫴��ս������temp
		if (isTerminal(theWord)) {
			temp.insert(theWord);
			return temp;
		}
		// ��ǰ�ַ������ս��������ݴ��ַ���first()�����Ƿ��Ц����ж��Ƿ���Ҫ����һ���ַ�
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
	// ��index == len˵�����ַ����е�ÿ���ַ���Ϊ���ս�����ҿ���Ϊ��
	if (index == len) {
		temp.insert(EPSILON);
	}
	return temp;
}

// follow(), �󵥸����ŵĿ��ܵ���һ���ս��
void follow() {
	// ��ʼʱ��ÿ�����ս����follow()���϶�Ϊ��
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		if (!isTerminal(*i))
			wordFollowSet[*i] = set<Symbol>();
	}
	// follow(S') = {��}
	wordFollowSet[BEGIN].insert(END);

	bool change = true;
	int len = productions.size();
	while (change) {
		change = false;
		for (int i = 0; i < len; ++i) {
			// ÿ������ʽ���Ҳ�ķ��Ŵ�
			vector<Symbol>& theRightSymbol = productions[i].second.getRightSymbol();
			int lenOfRight = theRightSymbol.size();
			// ��������ʽ�Ҳ�ķ��Ŵ���ÿ�����ţ�ֻ���Ƿ��ս��
			for (int j = 0; j < lenOfRight; ++j) {
				Symbol theWord = theRightSymbol[j];
				if (isTerminal(theWord)) { continue; }
				else {
					// ��set�в����ǰ���ս�����е�follow()���ϣ���������˷��ս������ķ��Ŵ���first()����
					set<Symbol>& theWordFollow = wordFollowSet[theWord];
					set<Symbol> joinSet = first((productions[i].second).allSymbolsAfterIndex(j));
					// ��joinSet�д���һЩtheWordFollow��������û�е��ս���������Ǽ��뵽theWordFollow������ȥ
					for (auto k = joinSet.begin(); k != joinSet.end(); ++k) {
						if (theWordFollow.find(*k) == theWordFollow.end() && !((*k) == EPSILON)) {
							change = true;
							theWordFollow.insert(*k);
						}
					}
					// ��joinSet����Ϊ�ţ�������ǰ���ս��������ַ�����first()���Ϻ��ЦŻ��߸��ַ��������Ϊ�ţ�
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


// ����Ŀ���ϵıհ�
void closure(ItemSet& theItemSet) {
	bool change = true;
	while (change) {
		change = false;
		vector<Item>& allItems = theItemSet.getItems();
		int len = allItems.size();
		// ����ÿ����Ŀ
		for (int i = 0; i < len; ++i) {
			Symbol theWordAfterDot = allItems[i].symbolAfterDot();
			// dot�������һ�����ս��
			if (!isTerminal(theWordAfterDot)) {
				int dotIndex = allItems[i].getDotPosition();
				// �˷��ս��������ַ������Լ�����first()����
				vector<Symbol> wordsAfter = allItems[i].allSymbolsAfterIndex(dotIndex);
				// ��ǰ�����ż���
				wordsAfter.push_back(allItems[i].getForeSymbol());
				set<Symbol> firstOfWordsAfter = first(wordsAfter);
				// ����ÿ������ʽ
				int lenOfProduction = productions.size();
				for (int j = 0; j < lenOfProduction; ++j) {
					Item theProduction = productions[j].second;
					// ����ʽ���󲿷��ս������Ŀdot��ķ��ս����ͬ
					if (theProduction.getLeftSymbol() == theWordAfterDot) {
						// ����ÿһ��b��FIRST����a��
						for (auto k = firstOfWordsAfter.begin(); k != firstOfWordsAfter.end(); ++k) {
							// ����һ���µ�item[B-->����,b]
							Item temp(theWordAfterDot, theProduction.getRightSymbol(), (*k), 0);
							// ����ʱ��closure��û�����item�������item����closure
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
	// ����I�е�������Ŀ
	for (int i = 0; i < len; ++i) {
		Item theItem = items[i];
		// ��һ����Ŀ��dot��ķ���Ϊ���ս������ΪtheSymbolʱ
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

// ������Ŀ�����DFA���Ѿ���itemSetGroup��������allSymbols��productions�Լ�BEGIN->S
void DFA(ItemSetGroup& itemSetGroup) {
	ofstream ofTransfer("transfer.txt", ios::out);
	if (!ofTransfer.is_open()) {
		cout << "��Transferʧ��" << endl;
	}
	ofTransfer.clear();

	// ����չ����
	queue<ItemSet> waitExpansion;
	// ��ȡ��Ŀ����ĸ������ݽṹ
	vector<ItemSet>& itemSets = itemSetGroup.getItemSets();
	ItemSet& I0 = itemSets[0];
	// ��BEGIN->S��closure���õ�I0;
	closure(I0);
	cout << "closure��:" << endl;
	cout << I0;
	// ��I0�������չ����
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
				// ��Ŀ�����д��ڸ���Ŀ��
				if (id >= 0) {}
				// ��Ŀ�����в����ڸ���Ŀ��������Ŀ������
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

		ofTransfer << "��Ч��Ŀ��I" << count++ << "��ת��:" << endl;
		for (auto j = thisTran.begin(); j != thisTran.end(); ++j) {
			ofTransfer << (*j).first << "---" << (*j).second << endl;
		}
		ofTransfer << endl;

		DFARecord.push_back(thisTran);
		waitExpansion.pop();
	}
	ofTransfer.close();
	// ��ӡDFA
	//cout << "DFA: " << endl;
	//for (int i = 0; i < DFARecord.size(); ++i) {
	//	cout << "��Ч��Ŀ��I" << i << "��ת��:" << endl;
	//	set<pair<Symbol, int>>& temp = DFARecord[i];
	//	for (auto j = temp.begin(); j != temp.end(); ++j) {
	//		cout << (*j).first << "---" << (*j).second << endl;
	//	}
	//	cout << endl;
	//}
}

void buildAnalysisTable(ItemSetGroup& itemSetGroup) {
	// ��ȡ��Ŀ����ĸ������ݽṹ
	vector<ItemSet>& itemSets = itemSetGroup.getItemSets();

	int itemSetSize = itemSetGroup.getItemSetSize();

	// ����action��goto���С
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

	// ������Ŀ�����е�ÿһ����Ŀ��
	for (int i = 0; i < itemSetSize; ++i) {
		// ����ÿ����Ŀ���е�ÿ����Ŀ
		vector<Item>& tempItems = itemSets[i].getItems();
		for (int j = 0; j < tempItems.size(); ++j) {
			Item& tempItem = tempItems[j];
			// �ǹ�Լ��Ŀ
			if (!tempItem.isReductionItem()) {
				Symbol charAfterDot = tempItem.symbolAfterDot();
				set<pair<Symbol, int>>& tempTran = DFARecord[i];
				// charAfterDot���ս��
				if (isTerminal(charAfterDot)) {
					for (auto k = tempTran.begin(); k != tempTran.end(); ++k) {
						if ((*k).first == charAfterDot) {
							action[i][charAfterDot].first = 1; // 1->S
							action[i][charAfterDot].second = (*k).second;
							break;
						}
					}
				}
				// charAfterDot�Ƿ��ս����
				else if (!isTerminal(charAfterDot)) {
					for (auto k = tempTran.begin(); k != tempTran.end(); ++k) {
						if ((*k).first == charAfterDot) {
							goTo[i][charAfterDot] = (*k).second;
							break;
						}
					}
				}
			}
			// ��Լ��Ŀ
			else {
				// ������Ŀ
				if (tempItem.getLeftSymbol() == BEGIN && tempItem.getForeSymbol() == END)
					action[i][END].first = 3; //3->ACK
				// ��Լ
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

// ����Ŀ������г�ʼ���������ķ�����ʶ����Ķ����
void initItemSetGroup(ItemSetGroup& itemSetGroup) {
	// ����
	ifstream fTranslateTable("translateTable.txt", ios::in);
	string s1, s2, s3;
	Symbol tempSymbol;
	int num;

	// ���뷭���ͷ��ű�
	if (!fTranslateTable.is_open())
		cout << "�޷����뷭�����" << endl;
	while (!fTranslateTable.eof()) {
		fTranslateTable >> s1 >> s2 >> s3;
		num = atoi(s1.c_str());
		numToStr.insert({ num, s3 });
		strToNum.insert({ s3, num });
		if (s2 == "T") allSymbols.insert(Symbol(Symbol::SymbolType::T, num));
		else allSymbols.insert(Symbol(Symbol::SymbolType::NT, num));
	}
	fTranslateTable.close();

	// ��������ļ�
	ifstream fStr("testStr.txt", ios::in);
	if (!fStr.is_open()) {
		cout << "�޷���������ļ�����" << endl;
		return;
	}


	while (!fStr.eof()) {
		fStr >> s1 >> s2 >> s3;
		num = atoi(s2.c_str());
		testStr.push_back(make_pair(searchSymbol(strToSymId(s1)), s3));
		columnNum.push_back(num);
	}
	fStr.close();
	testStr.push_back(make_pair(END,"����")); // ��������END
	columnNum.push_back(num);

	// �����ķ�(����ʽ)
	ifstream fGrammer("grammer.txt", ios::in);
	if (!fGrammer.is_open())
		cout << "�޷������ķ�" << endl;
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
		cout << "����" << symIdToStr((*i).getId()) << " ��first������";
		for (auto j = temp.begin(); j != temp.end(); ++j) {
			cout << symIdToStr((*j).getId()) << ' ';
		}
		cout << endl;
	}

	cout << endl;
	follow();
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		set<Symbol> temp = wordFollowSet[*i];
		cout << "����" << symIdToStr((*i).getId()) << " ��follow������";
		for (auto j = temp.begin(); j != temp.end(); ++j) {
			cout << symIdToStr((*j).getId()) << ' ';
		}
		cout << endl;
	}

	// ����ItemSetGroup
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

// ��ӡaction��goto��
void printTable() {
	cout << "action:\t";
	for (int i = 0; i < action[0].size(); ++i) cout << "\t";
	cout << " goto:" << endl << "\t";
	// �ȴ�ӡ���ս��
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

// �洢action��goto���ļ���
void storeTable() {
	//vector<map<Symbol, pair<int, int>>> action;
	//vector<map<Symbol, int>> goTo;


	ofstream ofAction("action.txt", ios::out);
	if (!ofAction.is_open()) {
		cout << "��tableʧ��" << endl;
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
		cout << "��tableʧ��" << endl;
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

// ���ļ��ж���action��goto��
void loadTable() {
	//vector<map<Symbol, pair<int, int>>> action;
	//vector<map<Symbol, int>> goTo;

	ifstream ifAction("action.txt", ios::in);
	if (!ifAction.is_open()) {
		cout << "��Action tableʧ��" << endl;
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
		cout << "��goTo tableʧ��" << endl;
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
			cout << "�ض��壡��" << endl;
		}
	}
};

struct Attribute {
	string Type;
	int value;
	Attribute() {
		Type = "";
		value = 0;
	}
};


SymbolTable mySymbolTable;

void process() {
	stack<int> stateStk;
	stack<pair<Symbol, Attribute>> symbolStk;
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
		// �������ű�
		tempSymbol = testStr[ip].first;
		describe = testStr[ip].second;
		column = columnNum[ip];

		state = stateStk.top();
		if (action[state][tempSymbol].first == 1) { // 1->S

			// �����µķ��ű��ɾȥ���ű�
			if (symIdToStr(tempSymbol.getId()) == "{") {
				cout << "\n{\n" << endl;
				mySymbolTable.buildNewTable();
			}
			else if (symIdToStr(tempSymbol.getId()) == "}") {
				cout << "\n}\n" << endl;
				mySymbolTable.removeTable();
			}


			cout << "�ƽ��ַ���\t" << tempSymbol << endl;
			//cout << "ȥ��״̬��\t" << action[state][tempSymbol].second << endl;
			stateStk.push(action[state][tempSymbol].second);
			Attribute tempAttribute;
			// �������id,���ҷ��ű����в�����

			if (symIdToStr(tempSymbol.getId()) == "id") {
				int t = mySymbolTable.searchRecord(describe);
				if (t == -1) {
					if (type == "") { // û��specifier�������������
						cout << "����δ����!!\n\n" << endl;
					}
					else { // ��type��ȡ���ͣ�������ű�
						cout << "���ű����" << describe << ",����Ϊ: " << type << "\n\n" << endl;
						tempAttribute.Type = type;
						mySymbolTable.joinTable(describe, type, column);
					}
				}
				else { // ���ű��д���
					tempAttribute.Type = mySymbolTable.symStk[t].type;
				}
			}
			// �������num
			else if (symIdToStr(tempSymbol.getId()) == "num") {
				tempAttribute.value = atoi(describe.c_str());
			}

			symbolStk.push(make_pair(tempSymbol, tempAttribute));
			treeStk.push(TreeNode(tempSymbol));
			++ip;
		}
		else if (action[state][tempSymbol].first == 2) { // 2->R
			int number = productions[action[state][tempSymbol].second].first;
			Item& production = productions[action[state][tempSymbol].second].second;
			cout << "��Լ��\t" << production << endl;

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
			//cout << "ת�Ƶ�״̬:" << goTo[state][production.getLeftSymbol()] << endl;


			TreeNode newNode = TreeNode(production.getLeftSymbol());
			newNode.joinVec(childVec);
			//cout << "�����½ڵ�:" << production.getLeftSymbol() << endl;

			treeStk.push(newNode);
			stateStk.push(goTo[state][production.getLeftSymbol()]);
			Attribute tempAttribute;
			// �����specifier������type
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
				cout << "һ��������������䣡����\n"  << endl;
				type = "";
			}
			else if (number == 58) { // Exp -> id
				int t = mySymbolTable.searchRecord(testStr[ip-1].second);
				if (t == -1)
						cout << "����δ����!!\t\t" << describe << endl;
				else { // ���ű��д���
					tempAttribute.Type = mySymbolTable.symStk[t].type;
					tempAttribute.value = mySymbolTable.symStk[t].value;
				}
			}
			else if (number == 59) { // Exp -> num
				cout << "Exp -> num  :" << computeVec[0].second.value << endl;
				tempAttribute.value = computeVec[0].second.value;
			}
			else if (number == 45) { // 45 Exp -> Exp + Exp
				// �����ж�����
				if (computeVec[0].second.Type != computeVec[2].second.Type) {
					cout << "���Ͳ�ƥ�䣡����\n" << endl;
				}
				cout << "Exp -> Exp + Exp  : " << computeVec[2].second.value <<  " + " << computeVec[0].second.value << endl;
				tempAttribute.value = computeVec[2].second.value + computeVec[0].second.value;
				tempAttribute.Type = computeVec[2].second.Type;
			}
			else if (number == 43) { // 43 Exp -> Exp = Exp
				tempAttribute.value = computeVec[0].second.value;
				tempAttribute.Type = computeVec[0].second.Type;
			}
			else if (number == 46) { // 46 Exp -> Exp - Exp
				if (computeVec[0].second.Type != computeVec[2].second.Type) {
					cout << "���Ͳ�ƥ�䣡����\n" << endl;
				}
				cout << "Exp -> Exp - Exp  : " << computeVec[2].second.value << " - " << computeVec[0].second.value << endl;
				tempAttribute.value = computeVec[2].second.value - computeVec[0].second.value;
				tempAttribute.Type = computeVec[2].second.Type;
			}
			else if (number == 47) { // 47 Exp -> Exp * Exp
				if (computeVec[0].second.Type != computeVec[2].second.Type) {
					cout << "���Ͳ�ƥ�䣡����\n" << endl;
				}
				cout << "Exp -> Exp * Exp  : " << computeVec[2].second.value << " * " << computeVec[0].second.value << endl;
				tempAttribute.value = computeVec[2].second.value * computeVec[0].second.value;
				tempAttribute.Type = computeVec[2].second.Type;
			}
			else if (number == 48) { // 48 Exp -> Exp / Exp
				if (computeVec[0].second.Type != computeVec[2].second.Type) {
					cout << "���Ͳ�ƥ�䣡����\n" << endl;
				}
				cout << "Exp -> Exp / Exp  : " << computeVec[2].second.value << " / " << computeVec[0].second.value << endl;
				tempAttribute.value = computeVec[2].second.value / computeVec[0].second.value;
				tempAttribute.Type = computeVec[2].second.Type;
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
			// ����յ�������ٴ�
			if (action[state][EPSILON].first == 1) {
				cout << "�ƽ��ַ���\t" << EPSILON << endl;
				//cout << "ȥ��״̬��\t" << action[state][EPSILON].second << endl;
				stateStk.push(action[state][EPSILON].second);
				symbolStk.push(make_pair(EPSILON, Attribute()));
				treeStk.push(TreeNode(EPSILON));
			}// �����Ҳ�����
			else {
				cout << "Error!\n" << endl;
				cout << tempSymbol << endl;
				errorRecord.push_back({ columnNum[ip], tempSymbol });
				// �����˳���ǰ״̬ջ��Ԫ��
				cout << "�˳�״̬ջ��" << stateStk.top() << endl;
				stateStk.pop();
				int popState;
				Symbol popSymbol;
				while (!symbolStk.empty() && !stateStk.empty() && stateStk.top() != 0) {
					popState = stateStk.top();
					popSymbol = symbolStk.top().first;
					// һ����˵��ķ��ս���Ǳ��ʽ����䡢��������̵�
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
					cout << "�����ַ�" << testStr[ip].first << endl;
					++ip;
				}
				if (ip >= testStr.size()) return;
				if (flag) {
					cout << "��ѭ��" << endl;
					return;
				}
			}
		}
	} while (1);
}
void printError() {
	cout << "����ͳ�ƣ�" << endl;
	cout << "����������" << errorRecord.size() << endl;
	cout << "�����к�\t" << "�������\t" << endl;
	for (int i = 0; i < errorRecord.size(); ++i)
		cout << errorRecord[i].first << "\t" << errorRecord[i].second << endl;
}


void main() {
	// ��Ч��Ŀ����
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
	// ��ӡaction��goto��

}