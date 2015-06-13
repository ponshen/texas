#include <iostream>
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include "Judge.h"

	/*
								�Ʀr�̦h�X�{M��			�ĤG�h�X�{N��
	Four of a kind(8):					4					1~3�S�t
	Full house(7):						3					3 or 2
	Three of a kind(4):					3					1	
	Two pairs(3)						2					2
	One pair(2)							2					1
	High Card(1)						1					1
	
	8�M7�����Τ��i�঳�P�ᶶ,���O�̤j�P
	��L�٭n�A�έp���M�P�wStraight,�ݦ��S���i�঳
	Royal straight flush(10), Straight flush(9), Flush(6), Straight(5)
	*/
PokerHand Judge::determineHand(Card (&cards)[7] )
{
	//�έp�Ʀr�X�{����,valueCount[]��[]���O�P���Ʀr,��0�ӭȤ��ϥ�
	//�έp���ƥ�,0���ϥ�,club:1,diamond:2,heart:3,spade:4
	int valueCount[14] ={0};
	int suitCount[5] = {0};

	for (int i = 0; i <= 6 ; i++)
		for (int j = 1; j <= 13; j++)
			if (j == cards[i].getValue())
			{
				valueCount[j]++;
				break;
			}

	//��X�X�{�̦h�����ƩM�ĤG�h������
	int maxCount = 0;
	int maxValue = 0;
	int secondCount = 0;

	for (int i = 1; i <= 13; i++)
		if (maxCount < valueCount[i])
		{
			maxCount = valueCount[i];
			maxValue = i;
		}

	for (int j = 1; j <= 13; j++)
		if (j != maxValue && secondCount < valueCount[j])
			secondCount = valueCount[j];

	//�έp�|�Ӫ�⪺�P��
	for (int i = 0; i <= 6 ; i++)
	{
		if (cards[i].getSuit() == Card::Club)
			suitCount[1]++;
		else if (cards[i].getSuit() == Card::Diamond)
			suitCount[2]++;
		else if (cards[i].getSuit() == Card::Heart)
			suitCount[3]++;
		else if (cards[i].getSuit() == Card::Spade)
			suitCount[4]++;
	}

	int theSuit = 0;
	bool boolSuit = false;
	for (int j = 1; j <= 4; j++)
		if (suitCount[j] >= 5)
		{
			boolSuit = true;
			theSuit = j;
		}


	/* 
	Four of a kind �� Full house������
	pick 5�i�P�Φ��̤j�P��,�Ǧ^PokerHand����,����determine()  
	*/
	Card pickList[5];	//���i�̫�npick���P

	if (maxCount == 4)
	{
		pickHand(cards, pickList, PokerHand::fourOfAKind, valueCount);
		PokerHand fourOfAKind(pickList, PokerHand::fourOfAKind);
		return fourOfAKind;
	}

	if (maxCount == 3 && (secondCount == 3 || secondCount == 2) )
	{
		pickHand(cards, pickList, PokerHand::fullHouse, valueCount);
		PokerHand fullHouse(pickList, PokerHand::fullHouse);
		return fullHouse;
	}

	/*
	�S����@���X�{5���H�W������
	��Straight�^��straight�P��,�S�����ܨ̧ǬO�T��,2 pairs, 1 pair, high card
	����@���X�{5���H�W������
	���P�wRoyalStraightFlush,�����ܶǦ^�P��
	�A�P�wStraightFlush,���S���N�OFlush
	*/
	if (!boolSuit)
	{
		//insertion sort,�q1�ƨ�13
		Card insert;
		for (int next = 1; next < 7; next++)
		{
			insert = cards[next];
			int moveItem = next;

			while ( (moveItem > 0) && (cards[moveItem - 1].getValue() > insert.getValue() ) )
			{
				cards[moveItem] = cards[moveItem - 1];
				moveItem--;
			}
			cards[moveItem] = insert;
		}//end sort

		int sValue = 0;
		bool s = checkStraight(valueCount, sValue);
		if(s)
		{
			pickHand(cards, pickList, PokerHand::straight, valueCount,theSuit,sValue);
			PokerHand straight(pickList, PokerHand::straight);
			return straight;
		}
		else
		{
			if (maxCount == 3)
			{
				//���B��maxValue�N��T�����Ʀr,�M�e��sValue���N�q���P
				pickHand(cards, pickList, PokerHand::threeOfAKind, valueCount,theSuit,maxValue);
				PokerHand threeOfAKind(pickList, PokerHand::threeOfAKind);
				return threeOfAKind;
			}
			if (maxCount == 2 && secondCount == 2)
			{
				pickHand(cards, pickList, PokerHand::twoPair, valueCount, theSuit);
				PokerHand twoPair(pickList, PokerHand::twoPair);
				return twoPair;
			}
			if (maxCount == 2 && secondCount == 1)
			{
				pickHand(cards, pickList, PokerHand::onePair, valueCount, theSuit, maxValue);
				PokerHand onePair(pickList, PokerHand::onePair);
				return onePair;
			}

			pickHand(cards, pickList, PokerHand::highCard, valueCount);
			PokerHand highCard(pickList, PokerHand::highCard);
			return highCard;
		}
	}//end if(!boolSuit)

	/*
	flushType == 2: Royal straight flush ; 1: Straight flush ; flushType == 0: Flush;
	vector<Card> suitSet �����P��⪺�P(5~7�i)
	sValue: straight���̤j���i,�ΨӨ�straight flush �� pickList
	*/
	else
	{
		vector <Card> suitSet;
		int sValue = 0;
		int flushType = checkFlushType(cards, theSuit, suitSet, sValue);
		
		/*
		Royal straight flush:
		�Ĥ@�ipick Ace,����q�Ʀr�j���ƨ�p��(KQJ10)
		*/
		if (flushType == 2)
		{
			pickList[0] = suitSet[0];
			pickList[1] = suitSet[suitSet.size() - 1];
			pickList[2] = suitSet[suitSet.size() - 2];
			pickList[3] = suitSet[suitSet.size() - 3];
			pickList[4] = suitSet[suitSet.size() - 4];
			
			pickHand(cards,pickList, PokerHand::royalStraightFlush, valueCount, theSuit);
			PokerHand royalStraightFlush(pickList,PokerHand::royalStraightFlush);
			return royalStraightFlush;
		}
		/*
		Straight flush:
		����X���l���_�Y�P,��b�Ĥ@�i,����q�j�ƨ�p
		*/
		else if (flushType == 1)
		{
			int index;
			for (unsigned int i = 0; i < suitSet.size(); i++)
				if (suitSet[i].getValue() == sValue)
					index = i;

			for (int j = 0; j <= 4; j++)
				pickList[j] = suitSet[index - j];

			pickHand(cards, pickList, PokerHand::straightFlush, valueCount,theSuit);
			PokerHand straightFlush(pickList,PokerHand::straightFlush);
			return straightFlush;
		}
		/*
		Flush:
		�p�G��Ace,�h�Ĥ@�i��Ace,�᭱�q�j�ƨ�p
		�S��Ace�h�����q�j�ƨ�p
		*/
		else
		{
			if (suitSet[0].getValue() == 1)
			{
				pickList[0] = suitSet[0];
				pickList[1] = suitSet[suitSet.size() - 1];
				pickList[2] = suitSet[suitSet.size() - 2];
				pickList[3] = suitSet[suitSet.size() - 3];
				pickList[4] = suitSet[suitSet.size() - 4];
			}
			else
			{
				pickList[0] = suitSet[suitSet.size() - 1];
				pickList[1] = suitSet[suitSet.size() - 2];
				pickList[2] = suitSet[suitSet.size() - 3];
				pickList[3] = suitSet[suitSet.size() - 4];
				pickList[4] = suitSet[suitSet.size() - 5];
			}
			pickHand(cards, pickList, PokerHand::flush, valueCount, theSuit);
			PokerHand flush(pickList, PokerHand::flush);
			return flush;
		}
	}



}//end Judge::determineHand()

void Judge::pickHand(Card (&cards)[7], Card (&pickList)[5], const int handType, const int (&valueCount)[14], const int theSuit,const int sValue)
{
	switch (handType)
	{
	/*
	Royal straight flush:
	�ˬd�O�_�X��,�S���X���N����
	*/
	case PokerHand::royalStraightFlush:
		{
			if (valueCount[1] == 0 || valueCount[13] == 0 || valueCount[12] == 0 || valueCount[11] == 0 || valueCount[10] == 0 || theSuit == 0)
				cout<<"ERROR at Royal Straight Flush."<<endl;
			break;
		}
	/*
	Straight flush:
	�ˬd�O�_�X��,�S���X���N����
	*/
	case PokerHand::straightFlush:
		{
			bool sferr = false; 
			for (int i = 1; i <= 4; i++)
				if (pickList[i].getValue() != pickList[i-1].getValue() - 1 )
					sferr = true;

			if (theSuit == 0 || sferr)
				cout<<"ERROR at Straight Flush."<<endl;
			break;
		}
	/*
	Four of a kind:
	�����w�X4�����Ʀr,�A�q�ѤU���Ʀr�̭��D�@�i�̤j��,�S�O�`�N1(Ace)�̤j,���O13,
	�p�G�P�}���ƼƱi�Ʀr�ۦP,�h�����i���M��ǤJ�x�}��card����,���|�Ӫ���
	*/
	case PokerHand::fourOfAKind:
		{
			int vFour = 0;
			int vLeft = 0;

			for (int i = 1; i <= 13; i++)
				if (valueCount[i] == 4)
					vFour = i;

			for (int j = 1; j <= 13; j++)
				if (valueCount[j] >= 1 && j != vFour && (j > vLeft && vLeft != 1) )
					vLeft = j;

			int pickIndex = 0;
			for (int k = 0; k <= 6; k++)
				if (cards[k].getValue() == vFour)
				{
					pickList[pickIndex] = cards[k];
					pickIndex++;
				}

			for (int l = 0; l <= 6; l++)
				if (cards[l].getValue() == vLeft)
					pickList[pickIndex] = cards[l];

			break;
		}//end case PokerHand::fourOfAKind:

	/*
	Full house:
	���T�ر��p:(331),(322),(3211)
	(331):�j����3,�p����2
	(322):��j��pair
	(3211):�S���D
	���P�w�O���@�زզ��A���O�B�z
	*/
	case PokerHand::fullHouse:
		{
			int countThree = 0;
			int valueThree[2] = {0};

			for (int i = 1; i <= 13; i++)
				if (valueCount[i] == 3)
				{
					valueThree[countThree] = i;
					countThree++;
				}

			int countTwo = 0;
			int valueTwo[2] = {0};

			for (int j = 1; j <= 13; j++)
				if (valueCount[j] == 2)
				{
					valueTwo[countTwo] = j;
					countTwo++;
				}

			int vThree = 0;
			int vTwo = 0;
			if (countThree == 2)
			{
				if (valueThree[0] != 1)
				{
					vThree = valueThree[1];
					vTwo = valueThree[0];
				}
				else
				{
					vThree = valueThree[0];
					vTwo = valueThree[1];
				}

			}
			else if (countTwo == 2)
			{
				vThree = valueThree[0];
				vTwo = (valueTwo[0] == 1 ? valueTwo[0] : valueTwo[1]);
			}
			else
			{
				vThree = valueThree[0];
				vTwo = valueTwo[0];
			}

			int pickIndex = 0;
			for (int i = 0; i <= 6; i++)
				if (cards[i].getValue() == vThree)
				{
					pickList[pickIndex] = cards[i];
					pickIndex++;
				}

			for (int j = 0; j <= 6; j++)
			{
				if (pickIndex >= 5)
					break;
				if (cards[j].getValue() == vTwo)
				{
					pickList[pickIndex] = cards[j];
					pickIndex++;
				}
			}

			break;
		}//end case PokerHand::fullHouse:
	/*
	Flush:
	�ˬd�O�_�X��,�S�X���N����
	*/
	case PokerHand::flush:
		{
			if (theSuit == 0)
				cout<<"ERROR at Flush."<<endl;
			break;
		}
	/*
	straight:
	�ǤJ���O�ӼƦr�ƧǹL��cards[7],sValue�Ostraight���̤j�Ʀr(Ace > King)
	��Ace: �� AKQJ10
	�S��Ace: �� sValue ~ (sValue - 4)
	*/
	case PokerHand::straight:
		{
			if (sValue == 1)
			{
				pickList[0] = cards[0];
				for (int i = 6; i > 0 ; i--)
				{
					switch(cards[i].getValue())
					{
					case 13:
						pickList[1] = cards[i];
						break;
					case 12:
						pickList[2] = cards[i];
						break;
					case 11:
						pickList[3] = cards[i];
						break;
					case 10:
						pickList[4] = cards[i];
						break;
					default:
						break;
					}
				}
			}
			else
			{
				for (int i = 6; i >= 0; i--)
				{
					int temp = cards[i].getValue();
					if (temp == sValue)
						pickList[0] = cards[i];
					else if (temp == sValue - 1)
						pickList[1] = cards[i];
					else if (temp == sValue - 2)
						pickList[2] = cards[i];
					else if (temp == sValue - 3)
						pickList[3] = cards[i];
					else if (temp == sValue - 4)
						pickList[4] = cards[i];
				}
			}
			break;
		}//end case PokerHand::straight
	/*
	Three of a kind:
	�ǤJ��cards[7]�ӼƦrsort�L(1~13)
	sValue�O�T�����Ʀr,��pick�T��,�Apick��i�̤j����i�P
	*/
	case PokerHand::threeOfAKind:
		{
			int index = 0;
			int t[3] = {0};
			for (int i = 0; i <= 6; i++)
				if(cards[i].getValue() == sValue)
				{
					pickList[index] = cards[i];
					t[index] = i;
					index++;
				}

			for (int j = 6; j >= 0; j--)
			{
				if (index >= 5)
					break;

				if (j == t[0] || j == t[1] || j ==t[2])
					continue;

				pickList[index] = cards[j];
				index++;
			}
			
			if (cards[0].getValue() == 1 && sValue != 1)
			{
				Card temp = pickList[3];
				pickList[3] = cards[0];
				pickList[4] = temp;
			}
			break;
		}//end case PokerHand::threeOfAKind:
	/*
	Two pair:
	�ǤJ��cards[7]�w�ӼƦr�Ƨ�
	����j��,�A�q�ѤU�T�i��@�i�Ʀr�̤j��
	��Ace pair�h�]���̤jpair,�Ʀr�̤j���]���ĤG
	��Ace��i�h��Ace���N�Ʀr�̤j����P
	*/
	case PokerHand::twoPair:
		{
			int largest = 0;
			int second = 0;

			for (int i = 2; i <= 13; i++)
				if (valueCount[i] == 2)
				{
					second = largest;
					largest = i;
				}

			if (valueCount[1] == 2)
			{
				second = largest;
				largest = 1;
			}

			int indexL = 0;
			int indexS = 2;
			for (int j = 6; j >= 0; j--)
			{
				if (cards[j].getValue() == largest)
				{
					pickList[indexL] = cards[j];
					indexL++;
				}
				else if (cards[j].getValue() == second)
				{
					pickList[indexS] = cards[j];
					indexS++;
				}
				else if (cards[j].getValue() > pickList[4].getValue())
					pickList[4] = cards[j];
			}
			if (valueCount[1] == 1 && cards[0].getValue() == 1)
				pickList[4] = cards[0];
			break;
		}//end case PokerHand::twoPair:

	/*
	One pair:
	�ǤJ��sValue�Opair���Ʀr
	pick��pair��Apick�T�i�Ʀr�̤j����P
	��P����Ace�����pAce�]���̤j��P,�Ʀr�̤j�ܲĤG,�Ʀr�ĤG�ܲĤT
	*/
	case PokerHand::onePair:
		{
			int indexP = 0;
			int indexH = 2;
			for (int i = 6; i >= 0; i--)
			{
				if (cards[i].getValue() == sValue)
				{
					pickList[indexP] = cards[i];
					indexP++;
				}
				else if (indexH <= 4)
				{
					pickList[indexH] = cards[i];
					indexH++;
				}
			}
			if (valueCount[1] == 1)
			{
				pickList[4] = pickList[3];
				pickList[3] = pickList[2];
				pickList[2] = cards[0];
			}
			break;
		}//end case PokerHand::onePair:

	/*
	High Card:
	��Ace: pick Ace,�ѤU�|�i�q�j��p
	�S��Ace: ���i���q�j��p
	*/
	case PokerHand::highCard:
		{
			if (valueCount[1] == 1)
			{
				pickList[0] = cards[0];
				int index = 1;
				for (int i = 6; i >= 0; i--)
				{
					if (index >= 5) 
						break;
					pickList[index] = cards[i];
					index++;
				}
			}
			else
			{
				int index = 0;
				for (int j = 6; j >= 0; j--)
				{
					if (index >= 5)
						break;
					pickList[index] = cards[j];
					index++;
				}
			}
			break;
		}
	//default:�i��o��N��Bug
	default:
		cout<<"Error at pickHand(), default run."<<endl;
		break;

	}//end switch
}//end pickHand()


/*
����P��⪺�ƨ��X�Ӱ��@��subset,�Asort subset�q1�ƨ�13,
�p��̤j�s��Ʀr����,����Royal, straight flush, flush�T��
*/
int Judge::checkFlushType(Card (&cards)[7],int theSuit, vector<Card> &suitSet, int& sValue)
{
	int flushType = 0;

	//���X�P��⪺�P
	for (int i = 0; i <= 6; i++)
		if (cards[i].getSuit() == theSuit)
			suitSet.push_back(cards[i]);

	//insertion sort,�q1�ƨ�13
	Card insert;
	for (unsigned int next = 1; next < suitSet.size(); next++)
	{
		insert = suitSet[next];
		int moveItem = next;

		while ( (moveItem > 0) && (suitSet[moveItem - 1].getValue() > insert.getValue() ) )
		{
			suitSet[moveItem] = suitSet[moveItem - 1];
			moveItem--;
		}
		
		suitSet[moveItem] = insert;
	}//end insertion sort

	/*
	if �s����� >= 4,�B�̤j�Ʀr�O13,�̤p�Ʀr�O1 : Royal straight flush, return 2
	else if �s����� >= 5 : straight flush, return 1
	else : flush, return 0
	*/
	int tempLength = 1;
	int sLength = 1;
	for (unsigned int i = 1; i < suitSet.size(); i++)
	{
		if (suitSet[i].getValue() == suitSet[i-1].getValue() + 1 )
		{
			tempLength++;

			if (tempLength > sLength)
			{
				sLength = tempLength;
				sValue = suitSet[i].getValue();
			}
		}
		else 
			tempLength = 1;
	}

	if (sLength >= 4 && sValue == 13 && suitSet[0].getValue() == 1)
		flushType = 2;
	else if (sLength >= 5)
		flushType = 1;

	return flushType;
}

/*
�p��ثe�s�����sLength�M�̤j�s�����maxLength
sLength == 0 if �S��King(13)
maxLength == 0 if �s�򤣺�5�i

if (sLength >= 4 && ��Ace) -> AKQJ10 straight
else if (maxLength >= 5) -> Straight
else �S��straight
*/
bool Judge::checkStraight(const int (&valueCount)[14], int& sValue)
{
	bool s = false;

	int sLength = 0;
	int maxLength = 0;
	for (int i = 1; i <= 13; i++)
	{
		if (valueCount[i] > 0)
		{
			sLength++;
			if (sLength >= 5)
			{
				maxLength = sLength;
				sValue = i;
			}
		}
		else
			sLength = 0;
	}

	if (sLength >= 4 && valueCount[1] > 0)
	{
		sValue = 1;
		s = true;
	}

	else if (maxLength >= 5)
		s = true;

	return s;
}