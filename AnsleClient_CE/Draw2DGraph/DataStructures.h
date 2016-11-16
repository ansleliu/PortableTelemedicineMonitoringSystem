#ifndef __DATASTRUCT_H_
#define __DATASTRUCT_H_
////////////////////////////////////////
#define ECG_SIZE   500
#define SPO2_SIZE  200
#define BP_SIZE    200
////////////////////////////////////////
typedef struct ECGDataNode
{
	int ECG[ECG_SIZE];
	struct ECGDataNode *next; 
}ECGDataNode;
typedef struct SPO2DataNode
{
	int SPO2[SPO2_SIZE];
	struct SPO2DataNode *next; 
}SPO2DataNode;
typedef struct BPDataNode
{
	int BP[BP_SIZE];
	struct BPDataNode *next; 
}BPDataNode;
typedef struct SendDataNode
{
	char* SendDate;
	struct SendDataNode *next;
}SendDataNode;
///////////////////////////////////////
ECGDataNode  *InitECGDataNode();
SPO2DataNode *InitSPO2DataNode();
BPDataNode   *InitBPDataNode();
SendDataNode *InitSendDataNode();
///////////////////////////////////////
ECGDataNode  *InitECGDataNode()
{
	ECGDataNode *L;
	L=(ECGDataNode*)malloc(sizeof(ECGDataNode));
	ASSERT(L!=NULL);
	memset(L->ECG,0,ECG_SIZE*sizeof(int));
	L->next = NULL;
	return L;
}
SPO2DataNode *InitSPO2DataNode()
{
	SPO2DataNode *L;
	L=(SPO2DataNode*)malloc(sizeof(SPO2DataNode));
	ASSERT(L!=NULL);
	memset(L->SPO2,0,SPO2_SIZE*sizeof(int));
	L->next = NULL;
	return L;
}
BPDataNode   *InitBPDataNode()
{
	BPDataNode *L;
	L=(BPDataNode*)malloc(sizeof(BPDataNode));
	ASSERT(L!=NULL);
	memset(L->BP,0,BP_SIZE*sizeof(int));
	L->next = NULL;
	return L;
}
SendDataNode *InitSendDataNode()
{
	SendDataNode *L;
	L=(SendDataNode*)malloc(sizeof(SendDataNode));
	ASSERT(L!=NULL);
	L->SendDate = NULL;
	L->next = NULL;
	return L;
}
#endif	//__DATASTRUCT_H_