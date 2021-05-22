class StrVec
{
public:
	StrVec():							//allocator成员进行默认初始化
		elements(nullptr),first_free(nullptr),cap(nullptr){}
	StrVec(const StrVec&);				//拷贝构造函数
	StrVec& operator=(const StrVec&);	//拷贝赋值运算符
	~StrVec();							//析构函数
	void push_back(const string&);		//拷贝元素
	size_t size() const { return first_free - elements; }
	size_t capacity() const { return cap - elements; }
	string* begin() const { return elements; }
	string* end() const { return first_free; }
private:
	static allocator<string>alloc;		//分配元素
	void chk_n_alloc()					//被添加元素的函数所使用	
	{
		if (size() == capacity()) reallocate();
	}
	pair<string*,string*>alloc_n_copy(const string*,const string*);	//工具函数，被拷贝构造函数、赋值运算符和析构函数所使用
	void free();				//销毁元素并释放内存
	void reallocate();
	string* elements;
	string* first_free;
	string* cap;
};
void StrVec::push_back(const string& s)
{
	chk_n_alloc();						//确保有空间容纳新元素
	alloc.construct(first_free++, s);	//在first_free指向的元素中构造s的副本
}
pair<string*, string*>StrVec::alloc_n_copy(const string* b, const string* e)
{
	//分配空间保存给定范围中的元素
	auto data = alloc.allocate(e - b);
	//初始化并返回一个pair，该pair由data和uninitialized_copy的返回值构成
	return { data,uninitialized_copy(b,e,data) };
}
void StrVec::free()
{
	//不能传递给deallocate一个空指针，如果elements为0，函数什么也不做
	if (elements)
	{
		//逆序销毁旧元素
		for (auto p = first_free; p != elements;)
			alloc.destroy(--p);
		alloc.deallocate(elements, cap - elements);
	}
}
StrVec::StrVec(const StrVec& s)
{
	//调用alloc_n_copy分配空间以容纳与s中一样多的元素
	auto newdata = alloc_n_copy(s.begin(), s.end());
	elements = newdata.first;
	first_free = cap = newdata.second;
}
StrVec::~StrVec() { free(); }
StrVec& StrVec::operator=(const StrVec& rhs)
{
	//调用alloc_n_copy分配内存，大小与rhs中元素占用空间一样多
	auto data = alloc_n_copy(rhs.begin(), rhs.end());
	free();
	elements = data.first;
	first_free = cap = data.second;
	return *this;
}
//reallocate函数应该：为一个新的更大的string数组分配内存
//在内存空间的前一部分构造对象，保存现有元素
//销毁原内存空间中的元素，并释放这块内存
//移动构造函数：将资源从给定对象“移动”而不是拷贝到正在创建的对象
//可以假定string的移动构造函数进行了指针的拷贝，而不是为字符分配内存空间然后拷贝字符
void StrVec::reallocate()
{
	//将分配当前大小两倍的内存空间
	auto newcapacity = size() ? 2 * size() : 1;
	//分配新内存
	auto newdata = alloc.allocate(newcapacity);
	//将数据从旧内存移动到新内存
	auto dest = newdata;		//指向新数组中下一个空闲位置
	auto elem = elements;		//指向旧数据中下一个元素
	for (size_t i = 0; i != size(); ++i)
	{
		alloc.construct(dest++, std::move(*elem++));
	}
	free();
	elements = newdata;
	first_free = dest;
	cap = elements + newcapacity;
}
