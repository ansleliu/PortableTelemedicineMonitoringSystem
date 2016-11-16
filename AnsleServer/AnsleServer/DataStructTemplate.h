#ifndef __DATASTRUCTTEMPLATE_H_
#define __DATASTRUCTTEMPLATE_H_
///////////////////////////////////////////////////////////////////////////////////////
//链表包装类模板类定义
////////////////////////////////////////////////////////////////////////////////////////
template < class LIST, class ITEM, int T>
class CDataStructTemplate
{
public:
	CDataStructTemplate()
	{
		TRACE0("CDataStructTemplate()\n");
		Init();
	}
	~CDataStructTemplate()
	{
		TRACE0("~CDataStructTemplate()\n");
		RemoveAll();
	}
protected:
	LIST	m_list;
	LIST	m_blank_list;
public:
	/*********************************************************************************
	功能：初始化m_blank_list
	说明：申请T个ITEM结点并放入m_blank_list
	用途：初始化
	参数：
	返回：
	*********************************************************************************/
	void Init()
	{
		ASSERT(T>0);
		for(int i=0;i<T;i++)
		{
			ITEM *pitem=new ITEM;
			m_blank_list.AddTail(pitem);
		}
	}
	/*********************************************************************************
	功能：清除所有内存
	说明：
	用途：
	参数：
	返回：
	*********************************************************************************/
	void RemoveAll()
	{
		POSITION pos;

		pos=m_list.GetHeadPosition();
		while(pos!=NULL)
		{
			ITEM *pitem=m_list.GetNext(pos);
			delete pitem;
		}

		m_list.RemoveAll();

		pos=m_blank_list.GetHeadPosition();
		while(pos!=NULL)
		{
			ITEM *pitem=m_blank_list.GetNext(pos);
			delete pitem;
		}
		m_blank_list.RemoveAll();
	}
	/*********************************************************************************
	功能：取得m_list中头结点的位置
	说明：
	用途：遍历m_list表
	参数：
	返回：成功，头结点POSITION；失败，NULL
	*********************************************************************************/
	POSITION GetHeadPosition()
	{
		return m_list.GetHeadPosition();
	}
	/*********************************************************************************
	功能：从m_list中取得下一个结点
	说明：根据输入的pos,返回相应的ITEM，同时将pos符值为下一结点的位置
	用途：遍历m_list表
	参数：pos:请求返回的结点位置
	返回：成功，ITEM；失败，NULL
	*********************************************************************************/
	ITEM *GetNext(POSITION &pos)
	{
		ASSERT(pos!=NULL);
		return m_list.GetNext(pos);
	}
	/*********************************************************************************
	功能：从m_list中取得下一个结点，放入m_blank_list中
	说明：如果有下一个结点，从m_list链表中删除该节点，并取得返回。否则返回NULL
	用途：从消息队列中取得一个消息，并清除该消息
	参数：无
	返回：成功，ITEM；失败，NULL
	*********************************************************************************/
	ITEM *GetNext()
	{
		ITEM *pitem=NULL;
		if(m_list.GetCount()>0)
		{
			pitem=m_list.RemoveHead();
			m_blank_list.AddTail(pitem);
		}
		return pitem;
	}
	/*********************************************************************************
	功能：取得下一个空节点，如果m_blank_list已空，则取得m_list的头节点，并放入m_list尾部
	说明：
	用途：滚动显示
	参数：
	返回：成功，ITEM；不会失败
	*********************************************************************************/
	ITEM *GetOwnCycleNext()
	{
		ASSERT(m_blank_list.GetCount()>0||m_list.GetCount()>0);
		ITEM *pitem=NULL;
		if(m_blank_list.GetCount()>0)
		{
			pitem=m_blank_list.RemoveHead();
			m_list.AddTail(pitem);
		}
		else
		{
			pitem=m_list.RemoveHead();
			m_list.AddTail(pitem);
		}
		return pitem;
	}
	/*********************************************************************************
	功能：从m_list中删除一个结点，并放到空表中
	说明：根据位置移结点
	用途：将m_list中的结点移到m_blank_list中
	参数：pos:要移的结点位置
	返回：成功，ITEM；失败，NULL
	*********************************************************************************/
	ITEM *RemoveAt(POSITION pos)
	{
		ASSERT(pos!=NULL);
		ITEM *pitem=NULL;
		pitem=m_list.GetAt(pos);
		m_list.RemoveAt(pos);
		m_blank_list.AddTail(pitem);
		return pitem;
	}
	/*********************************************************************************
	功能：从m_list中删除一个结点，并放到空表中
	说明：根据结点移结点
	用途：将m_list中的结点移到m_blank_list中
	参数：pitem
	返回：成功，ITEM；失败，NULL
	*********************************************************************************/
	ITEM *RemoveAt(ITEM *pitem)
	{
		ASSERT(pitem!=NULL);
		POSITION pos=m_list.Find(pitem);
		ASSERT(pos!=NULL);
		m_blank_list.AddTail(pitem);
		m_list.RemoveAt(pos);
		return pitem;
	}
	/*********************************************************************************
	功能：从空表中得到一个空节点，如空表中已经无可用结点，就再申请新的节点
	说明：无论如果，都能返回一个空结点
	用途：增加新结点
	参数：
	返回：成功，ITEM；不会失败
	*********************************************************************************/
	ITEM *GetBlank()
	{
		ITEM *pitem=NULL;
		if(m_blank_list.GetCount()==0)
		{
			pitem=new ITEM;
		}
		else
		{
			pitem=m_blank_list.RemoveHead();
		}

		m_list.AddTail(pitem);
		ASSERT(pitem!=NULL);
		return pitem;
	}
	void PutToBlank()
	{
		POSITION pos=m_list.GetHeadPosition();
		while(pos!=NULL)
		{
			ITEM *pitem=m_list.GetNext(pos);
			m_blank_list.AddTail(pitem);
		}
		m_list.RemoveAll();
	}
};

#endif //__DATASTRUCTTEMPLATE_H_