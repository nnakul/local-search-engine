
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define		MAXLEN 		101
#define		ALPHABET	36
#define		STOPCOUNT	77
#define		PENALTY_S	2
#define		PENALTY_I	1
#define		PENALTY_D	1

typedef struct node {
	int KEY ;
	struct node* NEXT ;
} LIST ;

typedef struct DOC {
	char WORD[MAXLEN] ;
	int ID ;
	int FREQ ;
	LIST* POS ;
	struct DOC* NEXT ;
} DOC ;

typedef struct TRIE {
	DOC* FILES ;
	struct TRIE* CHILDREN[ALPHABET] ;
} PTRIE ;

typedef struct STRIE {
	DOC* FILES ;
	struct STRIE* CHILDREN[ALPHABET] ;
} STRIE ;

typedef struct {
	int S ;
	DOC** TABLE ;
} HASHMAP ;

typedef struct {
	char WORD[MAXLEN] ;
	int FREQ ;
	LIST* POS ;
	int ID ;
	int RELEVANCE ;
} SEARCH_RESULT ;

PTRIE INIT_PTRIE ( ) ;
STRIE INIT_STRIE ( ) ;
void FILTER ( char* ) ;
int HASH_TRIE ( char ) ;
HASHMAP INIT_TABLE ( int ) ;
bool IS_ALPHA_NUMERIC ( char ) ;
int HASH ( char* , int ) ;
int MIN ( int , int ) ;
char INVERSE_HASH ( int ) ;
int IS_PRESENT ( HASHMAP* , char* ) ;
int EDIT_DISTANCE ( char* , char* ) ;
bool IS_STOPWORD ( char* , char[][MAXLEN] ) ;
bool IS_LESS_THAN ( SEARCH_RESULT , SEARCH_RESULT ) ;
void INSERT_INTO_PTRIE ( PTRIE* , char* , int , int ) ;
void INSERT_INTO_STRIE ( STRIE* , char* , int , int ) ;
void SWAP ( SEARCH_RESULT* , SEARCH_RESULT* ) ;
void QUICK_SORT ( SEARCH_RESULT* , int , int ) ;
bool IS_EQUAL ( SEARCH_RESULT , SEARCH_RESULT ) ;
int REMOVE_DUPLICATES ( SEARCH_RESULT* , int ) ;
void SORT_RESULTS ( SEARCH_RESULT* , int , int ) ;
void INSERT_INTO_HASHMAP ( char* , HASHMAP* , int , int ) ;
void SEARCH_PTRIE ( PTRIE , char* , SEARCH_RESULT* , int* ) ;
void SEARCH_STRIE ( STRIE , char* , SEARCH_RESULT* , int* ) ;
void GET_WORDS_P ( PTRIE , char* , int , SEARCH_RESULT* , int* , char* ) ;
void GET_WORDS_S ( STRIE , char* , int , SEARCH_RESULT* , int* , char* ) ;
void READ_FILE ( int , HASHMAP* , TRIE* , STRIE* , char[][MAXLEN] ) ;
void SEARCH_QUERY ( HASHMAP* , PTRIE , STRIE , SEARCH_RESULT* , int* , char[][MAXLEN] ) ;

clock_t CRAWL_S , CRAWL_E , SEARCH_S , SEARCH_E ;
int main ()
{
	HASHMAP H = INIT_TABLE( 20011 ) ;
	PTRIE T = INIT_PTRIE() ;
	STRIE ST = INIT_STRIE() ;
	char STOPWORD[][MAXLEN] = {
		"the" , "that" , "which" , "what" , "when" , "there" , "here" , "their" ,
		"his" , "her" , "them" , "she" , "and" , "are" , "have" , "had" , "has" ,
		"was" , "were" , "you" , "for" , "from" , "its" , "it's" , "whom" , "who" ,
		"been" , "under" , "inside" , "your" , "yours" , "very" , "without" , "few" ,
		"all" , "less" , "more" , "many" , "after" , "before" , "with" , "within" , "now" ,
		"other" , "others" , "this" , "hence" , "where" , "they" , "though" , "likes" ,
		"like" , "love" , "loves" , "most" , "how" , "about" , "can" , "any" , "nobody" ,
		"could" , "will" , "would" , "some" , "because" , "should" , "shall" , "must" , "may" ,
		"anybody" , "everybody" , "everyone" , "only" , "but" , "than" , "which" , "not"
	} ;
	int N = 20 , COUNT = 0 ;
	SEARCH_RESULT R[1009] ;
	char HEADER[] = "| STEMMED / NOISELESS WORDS | FILE ID | FREQUENCY | \t\t     POSITIONS     \t\t |" ;
	int HL = strlen(HEADER) ;
	CRAWL_S = clock() ;
	for ( int i=1 ; i<=N ; i++ )	READ_FILE( i , &H , &T , &ST , STOPWORD ) ;
	CRAWL_E = clock() ;
	printf ( "\n\n  SCANNED AND PROCESSED %d FILES IN %lf SECONDS " , N , (double)(CRAWL_E-CRAWL_S)/(double)CLOCKS_PER_SEC ) ;
	while( true )
	{
		COUNT = 0 ;
		SEARCH_QUERY( &H , T , ST , R , &COUNT , STOPWORD ) ;
		if ( ! COUNT )
		{
			printf( "\n\t+++ NO RESULTS FOUND +++" ) ;
			continue ;
		}
		QUICK_SORT( R , 0 , COUNT-1 ) ;
		COUNT = REMOVE_DUPLICATES( R , COUNT ) ;
		SORT_RESULTS( R , 0 , COUNT-1 ) ;
		printf("+") ;
		for ( int i=1 ; i<HL-28 ; i++ ) printf("--") ;
		printf("+") ;
		printf( "\n%s" , HEADER ) ;
		for ( int i=0 ; i<COUNT ; i++ )
		{
			printf( "\n| %-26s| %7d | %9d | " , R[i].WORD , R[i].ID , R[i].FREQ ) ;
			LIST* p = R[i].POS->NEXT ;
			int k = 0 ;
			while ( p )
			{
				if ( k % 7 == 0 && k )
				{
					printf("\t |\n|") ;
					for ( int j=0 ; j<27 ; j++ )	printf(" ") ;
					printf("|") ;
					for ( int j=0 ; j<9 ; j++ )	printf(" ") ;
					printf("|") ;
					for ( int j=0 ; j<11 ; j++ )	printf(" ") ;
					printf("| ") ;
				}
				printf(" %4d " , p->KEY) ; k++ ;
				p = p->NEXT ;
			}
			while ( k % 7 )	{ printf( " %4s " , " " ) ; k++ ; }
			printf("\t |") ;
		}
		printf("\n+") ;
		for ( int i=1 ; i<HL-28 ; i++ ) printf("--") ;
		printf("+") ;
		SEARCH_E = clock() ;
		printf ( "\n PRINTED %d RESULTS IN %lf SECONDS " , COUNT , (double)(SEARCH_E-SEARCH_S)/(double)CLOCKS_PER_SEC ) ;
	}
}

int MIN ( int x , int y )
{
	if ( x <= y )	return x ;
	return y ;
}

int EDIT_DISTANCE ( char* X , char* Y )
{
	int m = strlen(X) , n = strlen(Y) ;
	int** ed = ( int** ) calloc ( m+1 , sizeof(int*) ) ;
	for ( int i=0 ; i<=m ; i++ )
		ed[i] = ( int* ) calloc ( n+1 , sizeof(int) ) ;
	for ( int r=0 ; r<=m ; r++ )
		ed[r][n] = (m-r)*PENALTY_D ;
	for ( int c=0 ; c<=n ; c++ )
		ed[m][c] = (n-c)*PENALTY_I ;
		
	for ( int i=m-1 ; i>=0 ; i-- )
		for ( int j=n-1 ; j>=0 ; j-- )
		{
			int P = ( X[i] == Y[j] ) ? 0 : PENALTY_S ;
			ed[i][j] = MIN ( ed[i+1][j+1]+P , MIN ( ed[i][j+1]+PENALTY_I , ed[i+1][j]+PENALTY_D ) ) ;
		}
	int result = ed[0][0] ;
	free( ed ) ;
	return result ;
}

void SORT_RESULTS ( SEARCH_RESULT* arr , int si , int ei )
{
	if ( si >= ei ) return ;
	SEARCH_RESULT pivot = arr[si] ;
	int i = si ;
	int j = si + 1 ;
	while ( j<=ei )
	{
		if ( arr[j].RELEVANCE<pivot.RELEVANCE || 
			(arr[j].RELEVANCE==pivot.RELEVANCE && arr[j].FREQ>pivot.FREQ) || 
			(arr[j].RELEVANCE==pivot.RELEVANCE && arr[j].FREQ==pivot.FREQ && strcmp(arr[j].WORD,pivot.WORD)<0) || 
			(arr[j].RELEVANCE==pivot.RELEVANCE && arr[j].FREQ==pivot.FREQ && !strcmp(arr[j].WORD,pivot.WORD) && arr[j].ID<pivot.ID) )
			{  i++ ;	SWAP( arr+i , arr+j ) ;	}
		j++ ;
	}
	SWAP( arr+si , arr+i ) ;
	SORT_RESULTS( arr , si , i-1 ) ;
	SORT_RESULTS( arr , i+1 , ei ) ;
}

bool IS_EQUAL ( SEARCH_RESULT x , SEARCH_RESULT y )
{
	if ( strcmp(x.WORD,y.WORD) != 0 )	return false ;
	if ( x.ID != y.ID )	return false ;
	return true ;
}

int REMOVE_DUPLICATES ( SEARCH_RESULT* R , int n )
{
	SEARCH_RESULT Rn[n] ;
	SEARCH_RESULT t ;
	Rn[0] = R[0] ; int k = 1 ;
	t = Rn[0] ;
	for ( int i=1 ; i<n ; i++ )
	{
		if ( IS_EQUAL(R[i],t) )	continue ;
		Rn[k++] = R[i] ;
		t = Rn[k-1] ;
	}
	for ( int i=0 ; i<k ; i++ )
		R[i] = Rn[i] ;
	return k ;
}

void SWAP ( SEARCH_RESULT* p , SEARCH_RESULT* q )
{
	SEARCH_RESULT temp = *p ;
	*p = *q ;
	*q = temp ;
}

bool IS_LESS_THAN ( SEARCH_RESULT x , SEARCH_RESULT y )
{
	if ( strcmp(x.WORD,y.WORD) > 0 )	return false ;
	if ( strcmp(x.WORD,y.WORD) < 0 )	return true ;
	if ( x.ID < y.ID )	return true ;
	if ( x.ID > y.ID )	return false ;
	if ( x.RELEVANCE < y.RELEVANCE )	return true ;
	return false ;
}

void QUICK_SORT ( SEARCH_RESULT* arr , int si , int ei )
{
	if ( si >= ei ) return ;
	SEARCH_RESULT pivot = arr[si] ;
	int i = si ;
	int j = si + 1 ;
	while ( j<=ei )
	{
		if ( IS_LESS_THAN ( arr[j] , pivot ) )
		{
			i++ ;
			SWAP ( arr+i , arr+j ) ;
		}
		j++ ;
	}
	SWAP( arr+si , arr+i ) ;
	QUICK_SORT( arr , si , i-1 ) ;
	QUICK_SORT( arr , i+1 , ei ) ;
}

int HASH_TRIE ( char c )
{
	if ( c >= '0' && c <= '9' )	return c - '0' ;
	return 10 + c - 'a' ;
}

PTRIE INIT_PTRIE ( )
{
	TRIE T ;
	for ( int i=0 ; i<ALPHABET ; i++ )
	{
		T.CHILDREN[i] = NULL ;
	}
	return T ;
}

STRIE INIT_STRIE ( )
{
	STRIE T ;
	for ( int i=0 ; i<ALPHABET ; i++ )
	{
		T.CHILDREN[i] = NULL ;
	}
	return T ;
}

void INSERT_INTO_PTRIE ( PTRIE* T , char* word , int ID , int pos )
{
	int L = strlen( word ) , idx ;
	PTRIE *P = T , *Q = NULL ;
	for ( int i=0 ; i<L ; i++ )
	{
		idx = HASH_TRIE( word[i] ) ;
		if ( ! P->CHILDREN[idx] )
		{
			P->CHILDREN[idx] = ( PTRIE* ) malloc ( sizeof(PTRIE) ) ;
			for ( int j=0 ; j<ALPHABET ; j++ )
			{
				P->CHILDREN[idx]->CHILDREN[j] = NULL ;
				P->CHILDREN[idx]->FILES = ( DOC* ) malloc ( sizeof(DOC) ) ;
				P->CHILDREN[idx]->FILES->NEXT = NULL ;
			}
		}
		Q = P ;	P = P->CHILDREN[idx] ;
	}
	if ( ! P->FILES->NEXT || P->FILES->NEXT->ID != ID )
	{
		DOC* temp = P->FILES->NEXT ;
		P->FILES->NEXT = ( DOC* ) malloc ( sizeof(DOC) ) ;
		P->FILES->NEXT->ID = ID ;
		P->FILES->NEXT->FREQ = 0 ;
		P->FILES->NEXT->POS = ( LIST* ) malloc ( sizeof(LIST) ) ;
		P->FILES->NEXT->POS->NEXT = NULL ;
		P->FILES->NEXT->NEXT = temp ;
	}
	( P->FILES->NEXT->FREQ ) ++ ;
	LIST* p = P->FILES->NEXT->POS ;
	LIST* temp = p->NEXT ;
	p->NEXT = ( LIST* ) malloc ( sizeof(LIST) ) ;
	p->NEXT->NEXT = temp ;
	p->NEXT->KEY = pos ;
}

void INSERT_INTO_STRIE ( STRIE* T , char* word , int ID , int pos )
{
	int L = strlen( word ) , idx ;
	STRIE *P = T , *Q = NULL ;
	for ( int i=L-1 ; i>=0 ; i-- )
	{
		idx = HASH_TRIE( word[i] ) ;
		if ( ! P->CHILDREN[idx] )
		{
			P->CHILDREN[idx] = ( STRIE* ) malloc ( sizeof(STRIE) ) ;
			for ( int j=0 ; j<ALPHABET ; j++ )
			{
				P->CHILDREN[idx]->CHILDREN[j] = NULL ;
				P->CHILDREN[idx]->FILES = ( DOC* ) malloc ( sizeof(DOC) ) ;
				P->CHILDREN[idx]->FILES->NEXT = NULL ;
			}
		}
		Q = P ;	P = P->CHILDREN[idx] ;
	}
	if ( ! P->FILES->NEXT || P->FILES->NEXT->ID != ID )
	{
		DOC* temp = P->FILES->NEXT ;
		P->FILES->NEXT = ( DOC* ) malloc ( sizeof(DOC) ) ;
		P->FILES->NEXT->ID = ID ;
		P->FILES->NEXT->FREQ = 0 ;
		P->FILES->NEXT->POS = ( LIST* ) malloc ( sizeof(LIST) ) ;
		P->FILES->NEXT->POS->NEXT = NULL ;
		P->FILES->NEXT->NEXT = temp ;
	}
	( P->FILES->NEXT->FREQ ) ++ ;
	LIST* p = P->FILES->NEXT->POS ;
	LIST* temp = p->NEXT ;
	p->NEXT = ( LIST* ) malloc ( sizeof(LIST) ) ;
	p->NEXT->NEXT = temp ;
	p->NEXT->KEY = pos ;
}

int IS_PRESENT ( HASHMAP* H , char word[] )
{
	int idx = HASH( word , H->S ) ;
	for ( int i=0 ; i<H->S ; i++ )
	{
		if ( ! strcmp( H->TABLE[idx]->WORD , "#" ) )	return -1 ;
		if ( ! strcmp( H->TABLE[idx]->WORD , word ) )	return idx ;
		idx = ( idx + 1 ) % ( H->S ) ;
	}
	return -1 ;
}

char INVERSE_HASH ( int x )
{
	if ( x >= 0 && x <= 9 )	return '0'+x ;
	return x - 10 + 'a' ;
}

void GET_WORDS_P ( PTRIE P , char str[] , int idx , SEARCH_RESULT* R , int* COUNT , char* ORIG )
{
	for ( int i=0 ; i<ALPHABET ; i++ )
	{
		if ( ! P.CHILDREN[i] )	continue ;
		if ( P.CHILDREN[i]->FILES->NEXT )
		{
			str[idx] = INVERSE_HASH(i) ;
			str[idx+1] = '\0' ;
			DOC* Q = P.CHILDREN[i]->FILES->NEXT ;
			while ( Q )
			{
				SEARCH_RESULT t ;
				t.ID = Q->ID ;
				t.FREQ = Q->FREQ ;
				t.POS = Q->POS ;
				t.RELEVANCE = EDIT_DISTANCE( str , ORIG ) ;
				strcpy( t.WORD , str ) ;
				R[(*COUNT)++] = t ;
				Q = Q->NEXT ;
			}
			GET_WORDS_P( *(P.CHILDREN[i]) , str , idx+1 , R , COUNT , ORIG ) ;
			continue ;
		}
		str[idx] = INVERSE_HASH(i) ;
		GET_WORDS_P( *(P.CHILDREN[i]) , str , idx+1 , R , COUNT , ORIG ) ;
	}
}

void SEARCH_PTRIE ( PTRIE T , char word[] , SEARCH_RESULT* R , int* COUNT )
{
	TRIE P = T ;
	int i = 0 , L = strlen(word) ;
	char str[L] ; int k = 0 ;
	if ( L < 3 )	return ;
	if ( L < 6 )
	{
		while( i < L )
		{
			int idx = HASH_TRIE(word[i]) ;
			str[k++] = word[i] ;
			if ( ! P.CHILDREN[idx] )	return ;
			P = *(P.CHILDREN[idx]) ; i++ ;
		}
	}
	else
	{
		while( i <= L/2 )
		{
			int idx = HASH_TRIE(word[i]) ;
			str[k++] = word[i] ;
			if ( ! P.CHILDREN[idx] )	return ;
			P = *(P.CHILDREN[idx]) ; i++ ;
		}
	}
	str[k] = '\0' ;
	if ( P.FILES->NEXT )
	{
		DOC* Q = P.FILES->NEXT ;
		while ( Q )
		{
			SEARCH_RESULT t ;
			t.ID = Q->ID ;
			t.FREQ = Q->FREQ ;
			t.POS = Q->POS ;
			t.RELEVANCE = EDIT_DISTANCE( str , word ) ;
			strcpy( t.WORD , str ) ;
			R[(*COUNT)++] = t ;
			Q = Q->NEXT ;
		}
	}
	for ( int j=0 ; j<ALPHABET ; j++ )
	{
		if ( ! P.CHILDREN[j] )	continue ;
		if ( P.CHILDREN[j]->FILES->NEXT )
		{
			str[k] = INVERSE_HASH(j) ;
			str[k+1] = '\0' ;
			DOC* Q = P.FILES->NEXT ;
			while ( Q )
			{
				SEARCH_RESULT t ;
				t.ID = Q->ID ;
				t.FREQ = Q->FREQ ;
				t.POS = Q->POS ;
				t.RELEVANCE = EDIT_DISTANCE( str , word ) ;
				strcpy( t.WORD , str ) ;
				R[(*COUNT)++] = t ;
				Q = Q->NEXT ;
			}
			GET_WORDS_P( *(P.CHILDREN[j]) , str , k+1 , R , COUNT , word ) ;
			continue ;
		}
		str[k] = INVERSE_HASH(j) ;
		GET_WORDS_P( *(P.CHILDREN[j]) , str , k+1 , R , COUNT , word ) ;
	}
}

void GET_WORDS_S ( STRIE P , char str[] , int idx , SEARCH_RESULT* R , int* COUNT , char* ORIG )
{
	for ( int i=0 ; i<ALPHABET ; i++ )
	{
		if ( ! P.CHILDREN[i] )	continue ;
		if ( P.CHILDREN[i]->FILES->NEXT )
		{
			str[idx] = INVERSE_HASH(i) ;
			str[idx+1] = '\0' ;
			strcpy( str , strrev(str) ) ;
			DOC* Q = P.CHILDREN[i]->FILES->NEXT ;
			while ( Q )
			{
				SEARCH_RESULT t ;
				t.ID = Q->ID ;
				t.FREQ = Q->FREQ ;
				t.POS = Q->POS ;
				t.RELEVANCE = EDIT_DISTANCE( str , ORIG ) ;
				strcpy( t.WORD , str ) ;
				R[(*COUNT)++] = t ;
				Q = Q->NEXT ;
			}
			strcpy( str , strrev(str) ) ;
			GET_WORDS_S( *(P.CHILDREN[i]) , str , idx+1 , R , COUNT , ORIG ) ;
			continue ;
		}
		str[idx] = INVERSE_HASH(i) ;
		GET_WORDS_S( *(P.CHILDREN[i]) , str , idx+1 , R , COUNT , ORIG ) ;
	}
}

void SEARCH_STRIE ( STRIE T , char word[] , SEARCH_RESULT* R , int* COUNT )
{
	STRIE P = T ;
	int i = 0 , L = strlen(word) ;
	if ( L < 3 )	return ;
	strcpy( word , strrev( word ) ) ;
	char str[L] ; int k = 0 ;
	if ( L < 6 )
	{
		while( i < L )
		{
			int idx = HASH_TRIE(word[i]) ;
			str[k++] = word[i] ;
			if ( ! P.CHILDREN[idx] )	return ;
			P = *(P.CHILDREN[idx]) ; i++ ;
		}
	}
	else
	{
		while( i <= L/2 )
		{
			int idx = HASH_TRIE(word[i]) ;
			str[k++] = word[i] ;
			if ( ! P.CHILDREN[idx] )	return ;
			P = *(P.CHILDREN[idx]) ; i++ ;
		}
	}
	str[k] = '\0' ;
	char temp[MAXLEN] ;
	strcpy( temp , str ) ;
	temp[4] = '\0' ;
	if ( ! strcmp( temp , "noit" ) )	return ;
	if ( ! strcmp( temp , "ylla" ) )	return ;
	if ( ! strcmp( temp , "ytei" ) )	return ;
	if ( ! strcmp( temp , "nois" ) )	return ;
	if ( ! strcmp( temp , "seta" ) )	return ;
	temp[3] = '\0' ;
	if ( ! strcmp( temp , "gni" ) )	return ;
	if ( ! strcmp( temp , "yli" ) )	return ;
	if ( ! strcmp( temp , "yll" ) )	return ;
	if ( ! strcmp( temp , "suo" ) )	return ;
	if ( ! strcmp( temp , "luf" ) )	return ;
	if ( ! strcmp( temp , "msi" ) )	return ;
	if ( ! strcmp( temp , "yti" ) )	return ;
	if ( ! strcmp( temp , "eta" ) )	return ;
	if ( ! strcmp( temp , "edi" ) )	return ;
	if ( ! strcmp( temp , "ega" ) )	return ;
	temp[2] = '\0' ;
	if ( ! strcmp( temp , "de" ) )	return ;
	if ( ! strcmp( temp , "se" ) )	return ;
	if ( ! strcmp( temp , "la" ) )	return ;
	if ( ! strcmp( temp , "el" ) )	return ;
	if ( ! strcmp( temp , "es" ) )	return ;
	if ( P.FILES->NEXT )
	{
		DOC* Q = P.FILES->NEXT ;
		strcpy( str , strrev(str) ) ;
		while ( Q )
		{
			SEARCH_RESULT t ;
			t.ID = Q->ID ;
			t.FREQ = Q->FREQ ;
			t.POS = Q->POS ;
			t.RELEVANCE = EDIT_DISTANCE( str , word ) ;
			strcpy( t.WORD , str ) ;
			R[(*COUNT)++] = t ;
			Q = Q->NEXT ;
		}
		strcpy( str , strrev(str) ) ;
	}
	for ( int j=0 ; j<ALPHABET ; j++ )
	{
		if ( ! P.CHILDREN[j] )	continue ;
		if ( P.CHILDREN[j]->FILES->NEXT )
		{
			str[k] = INVERSE_HASH(j) ;
			str[k+1] = '\0' ;
			strcpy( str , strrev(str) ) ;
			DOC* Q = P.CHILDREN[j]->FILES->NEXT ;
			while ( Q )
			{
				SEARCH_RESULT t ;
				t.ID = Q->ID ;
				t.FREQ = Q->FREQ ;
				t.POS = Q->POS ;
				t.RELEVANCE = EDIT_DISTANCE( str , word ) ;
				strcpy( t.WORD , str ) ;
				R[(*COUNT)++] = t ;
				Q = Q->NEXT ;
			}
			strcpy( str , strrev(str) ) ;
			GET_WORDS_S( *(P.CHILDREN[j]) , str , k+1 , R , COUNT , word ) ;
			continue ;
		}
		str[k] = INVERSE_HASH(j) ;
		GET_WORDS_S( *(P.CHILDREN[j]) , str , k+1 , R , COUNT , word ) ;
	}
}

void SEARCH_QUERY ( HASHMAP* H , PTRIE T , STRIE ST , SEARCH_RESULT* R , int* COUNT , char SW[][MAXLEN] )
{
	char query[MAXLEN*900] ;
	printf("\n\nENTER QUERY : " ) ;
	scanf( " %[^\n]s" , query ) ;
	SEARCH_S = clock() ;
	char word[MAXLEN] , wordd[MAXLEN] ;
	int K = -1 ;
	while ( true )
	{
		if ( K>=0 && query[K] == '\0' )	break ;
		sscanf ( query+K+1 , " %s" , word ) ;
		if ( ! strcmp( word , "#QUIT" ) )
		{
			printf("\n\n\tGOODBYE...\n\n") ;
			exit(0) ;
		}
		K = K + strlen(word) + 1 ;
		FILTER( word ) ;
		if ( IS_STOPWORD(word , SW) )	continue ;
		int idx = IS_PRESENT( H , word ) ;
		strcpy( wordd , word ) ;
		if ( idx == -1 )
		{
			SEARCH_PTRIE( T , word , R , COUNT ) ;
			SEARCH_STRIE( ST , wordd , R , COUNT ) ;
			continue ;
		}
		DOC* p = H->TABLE[idx]->NEXT ;
		while ( p )
		{
			SEARCH_RESULT t ;
			t.ID = p->ID ;
			t.FREQ = p->FREQ ;
			t.POS = p->POS ;
			t.RELEVANCE = 0 ;
			strcpy( t.WORD , word ) ;
			R[(*COUNT)++] = t ;
			p = p->NEXT ;
		}
		SEARCH_PTRIE( T , word , R , COUNT) ;
		SEARCH_STRIE( ST , wordd , R , COUNT) ;
	}
}

HASHMAP INIT_TABLE ( int S )
{
	HASHMAP H ;
	H.S = S ;
	H.TABLE = ( DOC** ) calloc ( S , sizeof(DOC*) ) ;
	for ( int i=0 ; i<S ; i++ )
	{
		H.TABLE[i] = ( DOC* ) malloc ( sizeof(DOC) ) ;
		H.TABLE[i]->NEXT = NULL ;
		strcpy( H.TABLE[i]->WORD , "#" ) ;
	}
	return H ;
}

bool IS_ALPHA_NUMERIC ( char x )
{
	if ( x >= '0' && x <= '9' )	return true ;
	if ( x >= 'a' && x <= 'z' )	return true ;
	if ( x >= 'A' && x <= 'Z' )	return true ;
	return false ;
}

void FILTER ( char* str )
{
	for ( int i = 0 ; str[i] ; i++ )
  	{
  		if ( ! IS_ALPHA_NUMERIC(str[i]) )
  		{
  			strcpy( str+i , str+i+1 ) ;
  			i-- ;
  			continue ;
		}
  		str[i] = tolower(str[i]) ;
	}
}

int HASH ( char* str , int S )
{
	int H = 0 , a = 37 , l = strlen(str) ;
	for ( int i=0 ; i<l ; i++ )
		H = ( a*H + str[i] ) % S ;
	return H ;
}

bool IS_STOPWORD ( char* str , char SW[][MAXLEN] )
{
	if ( strlen(str) < 3 )	return true ;
	for ( int i=0 ; i<STOPCOUNT ; i++ )
		if ( ! strcmp( str , SW[i] ) )	return true ;
	return false ;
}

void INSERT_INTO_HASHMAP ( char* word , HASHMAP* H , int ID , int POS )
{
	int idx = HASH( word , H->S ) , i ;
	for ( i=0 ; i<H->S ; i++ )
	{
		if ( ! strcmp( H->TABLE[idx]->WORD , "#" ) )	break ;
		if ( ! strcmp( H->TABLE[idx]->WORD , word ) )	break ;
		idx = ( idx + 1 ) % ( H->S ) ;
	}
	strcpy( H->TABLE[idx]->WORD , word ) ;
	if ( ! H->TABLE[idx]->NEXT || H->TABLE[idx]->NEXT->ID != ID )
	{
		DOC* temp = H->TABLE[idx]->NEXT ;
		H->TABLE[idx]->NEXT = ( DOC* ) malloc ( sizeof(DOC) ) ;
		H->TABLE[idx]->NEXT->ID = ID ;
		H->TABLE[idx]->NEXT->FREQ = 0 ;
		H->TABLE[idx]->NEXT->POS = ( LIST* ) malloc ( sizeof(LIST) ) ;
		H->TABLE[idx]->NEXT->POS->NEXT = NULL ;
		H->TABLE[idx]->NEXT->NEXT = temp ;
	}
	( H->TABLE[idx]->NEXT->FREQ ) ++ ;
	LIST* p = H->TABLE[idx]->NEXT->POS ;
	LIST* temp = p->NEXT ;
	p->NEXT = ( LIST* ) malloc ( sizeof(LIST) ) ;
	p->NEXT->NEXT = temp ;
	p->NEXT->KEY = POS ;
}

void READ_FILE ( int ID , HASHMAP* H , PTRIE* T , STRIE* ST , char SW[][MAXLEN] )
{
	char file[] = "input_" ;
	char suff[3] ;
	sprintf( suff , "%d" , ID ) ;
	strcat( file , suff ) ;
	strcat( file , ".txt" ) ;
	FILE* fptr = fopen ( file , "r" ) ;
	char word[MAXLEN] ;
	int k = 1 ;
	while( ! feof(fptr) )
	{
		fscanf( fptr , " %s" , word ) ;
		FILTER( word ) ;
		if ( IS_STOPWORD(word , SW) )	continue ;
		INSERT_INTO_HASHMAP( word , H , ID , k ) ;
		INSERT_INTO_PTRIE( T , word , ID , k ) ;
		INSERT_INTO_STRIE( ST , word , ID , k ) ;
		k += strlen(word) + 1 ;
	}
	fclose( fptr ) ;
}
