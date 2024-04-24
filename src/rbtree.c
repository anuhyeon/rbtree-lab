#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void)
{
  // TODO: initialize struct if needed
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  if (p == NULL)
  { // 메모리할당 여부 확인
    printf("rbtree 메모리 할당 실패");
    return NULL;
  }

  p->nil = (node_t *)malloc(sizeof(node_t));
  if (p->nil == NULL)
  {
    printf("nil 노드 메모리 할당 실패");
    return NULL;
  }
  p->nil->color = RBTREE_BLACK;
  p->nil->key = 0;
  p->nil->parent = p->nil;
  p->nil->left = p->nil;
  p->nil->right = p->nil;
  p->root = p->nil; // 맨처음 아무런 노드가 없을때는 nil 노드가 root 노드임

  return p;
}

void rb_transplant(rbtree* t,node_t* u, node_t* v){ //u노드를 v로 대체
  if(u->parent == t->nil){
    t->root = v;
  }
  else if(u == u->parent->left){
    u->parent->left = v;
  }
  else{
    u->parent->right = v;
  }
  v->parent = u->parent;
}

void delete_rbtree(rbtree *t)
{
  // TODO: reclaim the tree nodes's memory
  free(t);
}
void left_rotate(rbtree* t, node_t* x){ // x는 회전할 때의 기준 노드
  node_t* y;
  y = x->right; // x위치에 올y 설정
  x->right = y->left; // y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 옮김
  if(y->left != t->nil){ // y의 왼쪽 자식이 nil 노드가 아니면
    y->left->parent = x; //y의 왼쪽 자식은 낙동강 오리알이 되므로 자기의 오른쪽 자식이 부모가 된 x노드의 오른쪽 자식으로 입양시켜줌
  } 
  y->parent = x->parent; // x의 부모를 y의 부모로 (y는 원래 x의 오른쪽 자식이었음) 
  if(x->parent == t->nil){ // x가 root 노드이면
    t->root = y; // y를 root노드로 업데이트
  }
  //x의 부모를 y의 부모로 옮겼으니 y를 부모의 왼쪽 자식으로 넣어야하는지 오른쪽 자식으로 넣어야하는지 check
  else if(x == x->parent->left){ //x가 부모의 왼쪽 자식이면
    x->parent->left = y; //y를 x의 부모의 왼쪽 자식으로 넣어줌
  }
  else{ //x가 부모의 오른쪽 자식이면
    x->parent->right = y; // y를 x의 부모의 오른쪽 자식으로 넣어줌
  }
  //아래 두줄은 제일 마지막에 넣어줘야함
  y->left = x; // x를 y의 왼쪽 자식으로 넣는다. 
  x->parent = y; 
}

void right_rotate(rbtree* t, node_t* x){
    node_t *y = x->left; // y는 회전의 중심이 되는 x의 왼쪽 자식
    x->left = y->right; // y의 오른쪽 서브트리를 x의 왼쪽 서브트리로 설정
    if (y->right != t->nil) {
        y->right->parent = x;
    }
    y->parent = x->parent; // y의 부모를 x의 부모로 설정
    if (x->parent == t->nil) { // x가 루트 노드였다면, y를 새로운 루트로 설정
        t->root = y;
    } else if (x == x->parent->right) { // x가 부모의 오른쪽 자식이었다면, y를 그 위치에 설정
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x; // x를 y의 오른쪽 자식으로 설정
    x->parent = y; // x의 부모를 y로 설정
}

node_t *insert_fixup(rbtree *t, node_t *z){
  node_t *y;
  while (z->parent->color == RBTREE_RED){ // 새로 삽입된 노드의 부모가 red일 경우에만 반복문 실행 (삽입에서 case 1,2,3 모두 삽입된 부모가 모두 red이기 떄문에 red의 불연속 속성에 위배되어 수정하는 것)
    if (z->parent == z->parent->parent->left){
      y = z->parent->parent->right;
      if (y->color == RBTREE_RED){ // case1에 해당하는 경우 자식둘과 부모색을 바꿔주면됌 (색 반전 시켜줘기)
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; // z의 부모의 부모가 red로 바뀌었으니깐 z의 위치를 z.p.p로 바꾸어서 다시 속성위반 여부 확인
      }
      else{
        if(z == z->parent->right){ //case2에 해당하는 경우 case3 형태로 만들어주기
          z = z->parent; //z의 부모를 기준으로 rotate///  여기서 left_rotate에 바로 z->parent를 인자로 넣어줄 수있지 않냐라는 의문이 생길 수 있는데 넣게되면 case3재조정하기 직전에 z의 위치를 좀 많이 수정해줘야함
          left_rotate(t,z);
        }
        // z의 부모의 색과 z의 할아버지 색을 회전하기 전에 change case3에 해당
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t,z->parent->parent);
      }
    }
    else{
      y = z->parent->parent->left;
      if (y->color == RBTREE_RED){ // case1에 해당하는 경우 자식둘과 부모색을 바꿔주면됌 (색 반전 시켜줘기)
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; // z의 부모의 부모가 red로 바뀌었으니깐 z의 위치를 z.p.p로 바꾸어서 다시 속성위반 여부 확인
      }
      else{
        if(z == z->parent->left){ //case2에 해당하는 경우 case3 형태로 만들어주기
          z = z->parent; //z의 부모를 기준으로 rotate///  여기서 left_rotate에 바로 z->parent를 인자로 넣어줄 수있지 않냐라는 의문이 생길 수 있는데 넣게되면 case3재조정하기 직전에 z의 위치를 좀 많이 수정해줘야함
          right_rotate(t,z);
        }
        // z의 부모의 색과 z의 할아버지 색을 회전하기 전에 change case3에 해당
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t,z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
  return t->root;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // TODO: implement insert
  node_t *y = t->nil;
  node_t *x = t->root;
  while (x != t->nil)
  {
    y = x; // y에 계속 x의 위치 따로 업데이트 y는 tmp 역할을 한다고 볼 수 있음(y는 insert하려는 노드의 부모가 됨). x는 cur 역할을 한다고 볼 수 있음.
    if (key < x->key)
    {
      x = x->left; // x는 x의 왼쪽 자식으로 이동
    }
    else
    {
      x = x->right;
    }
  }
  node_t *new_node = (node_t *)malloc(sizeof(node_t)); // 새로운 노드 생성
  if (new_node == NULL)
  { // 메모리 할당 여부 체크
    return NULL;
  }
  // new_node 초기화
  new_node->key = key;
  new_node->parent = y; // new_node의 부모는 y --> 만약 루트노드가 없는 경우라면 new_node의 부모는 nil이 됨
  new_node->left = t->nil;
  new_node->right = t->nil;
  new_node->color = RBTREE_RED;

  if (y == t->nil)
  {                     // 만약 루트노드가 없는 경우라면 new_node의 부모는 위에서 nil로 초기화를 해줬을테고 이제 부모가 nil이면 해당 노드는 root노드이니깐 이거에 맞게 초기화 진행
    t->root = new_node; // new_node가 루트노드임.
    t->root->color = RBTREE_BLACK;
  }
  else if (new_node->key < y->key)
  {
    y->left = new_node;
  }
  else
  {
    y->right = new_node;
  }
  
  return insert_fixup(t,new_node);
}



  node_t *rbtree_find(const rbtree *t, const key_t key){
    // TODO: implement find
     node_t *current = t->root; // 탐색을 시작할 현재 노드를 루트로 설정.

    // 탐색할 노드가 nil 노드가 아니고, 찾고자 하는 키를 가진 노드를 찾을 때까지 반복.
    while (current != t->nil) {
    if (key == current->key) {
      // 찾고자 하는 키를 가진 노드를 찾았을 경우 해당 노드를 반환.
      return current;
    } else if (key < current->key) {
      // 찾고자 하는 키가 현재 노드의 키보다 작을 경우 왼쪽 자식으로 이동.
      current = current->left;
    } else {
      // 찾고자 하는 키가 현재 노드의 키보다 클 경우 오른쪽 자식으로 이동.
      current = current->right;
    }
  }
  // 찾고자 하는 키를 가진 노드를 찾지 못한 경우 nil 노드를 반환.
  // 혹은 찾고자 하는 키가 트리에 존재하지 않을 경우.
    return NULL;
}

  node_t *rbtree_min(const rbtree *t)
  {
    // TODO: implement find
    node_t *node = t->root;
    if (node == t->nil) { // 트리가 비어있거나 SENTINEL만 있는 경우
      return NULL;
    }

    // 왼쪽 자식 노드가 SENTINEL이 아닐 때까지 계속 왼쪽으로 이동
    while (node->left != t->nil) {
      node = node->left;
    }

    return node;
  }

  node_t *rbtree_max(const rbtree *t)
  {
    // TODO: implement find
    node_t *node = t->root;
    if (node == t->nil) { // 트리가 비어있거나 SENTINEL만 있는 경우
      return NULL;
    }

    // 오른쪽 자식 노드가 SENTINEL이 아닐 때까지 계속 오른쪽으로 이동
    while (node->right != t->nil) {
      node = node->right;
    }

    return node;
  }

  node_t* tree_minimum(rbtree* t,node_t* node){
    // node는 successor를 찾기 위한 시작 노드
    // 오른쪽 서브트리에서 가장 작은 값을 찾아서 반환
    while (node->left != t->nil) {
        node = node->left;
    }
    return node;
  
  }

  void erase_fixup(rbtree* t, node_t* x){ //x는 extra black이 붙는 노드( x = doubly black )
    node_t* w;
    while(x != t->root && x->color == RBTREE_BLACK){ // x->color = red인경우는 red and black인 상태이므로 더블리 블랙이 사라지게 되므로 반복문 탈출하면됌( 레드앤블랙은 블랙으로 바꿔주면됌)
      if(x == x->parent->left){ // 더블리 노드가 왼쪽에 위치하는 경우
        w = x->parent->right; // w는 x의 형제
        if(w->color == RBTREE_RED){ // case 1 인 경우 -> 형제가 red인 경우
          w->color = RBTREE_BLACK; // 회전하기 전에 형제의 색을 black으로 부모의 색을 red로 바꿔주기
          x->parent->color = RBTREE_RED;
          left_rotate(t,x->parent);
          w = x->parent->right; // 회전해서 x의 형제가 바뀌었으므로 형제(w)를 다시 update
        }
        if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){ //case 2인 경우
          w->color = RBTREE_RED; // black을 부모한테 주고 자기 자신은 red로 바뀜 + x또한 extra black을 부모한테 줌으로써 부모는 왼쪽오른쪽 자식들로 부터 black을 두개 받아 extra black하나가 생기게됨. 따라서 부모가 더블리 블랙이됨.
          x = x->parent; // 부모가 더블리 블랙이 되었으므로 x의 위치를 업데이트 (x는 더블리 블랙을 가리키는 역할을 함.)
        }
        else{ 
          if(w->right->color == RBTREE_BLACK){ // 바로 위 if에 대한 else 임. -> case 3
            w->left->color = RBTREE_BLACK; // 형제의 왼쪽 자식의 색을 블랙으로 바꿔줌
            w->color = RBTREE_RED;  // 형제의 색을 red로 바꿔줌  --> 회전하기 전에 색을 바꿔주는 작업
            right_rotate(t,w);
            w = x->parent->right; // 회전하면 형제 바뀌므로 다시 형제 업데이트
          }
          w->color = x->parent->color; //case4 --> 형제의 색을 부모의 색으로
          x->parent->color = RBTREE_BLACK; // 부모 색 black으로
          w->right->color = RBTREE_BLACK; // 형제의 외측자녀(red)를 black으로 바꿔줌
          left_rotate(t,x->parent); // 회전
          x = t->root; // while문 종료
        }
      }
      else{ //더블리 노드가 오른쪽에 위치하는 경우
        w = x->parent->left;
        if(w->color == RBTREE_RED){
          w->color = RBTREE_BLACK;
          x->parent->color = RBTREE_RED;
          right_rotate(t,x->parent);
          w = x->parent->left;
        }
        if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
          w->color = RBTREE_RED;
          x = x->parent;
        }
        else{
          if(w->left->color == RBTREE_BLACK){
            w->right->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            left_rotate(t,w);
            w = x->parent->left;
          }
          w->color = x->parent->color;
          x->parent->color = RBTREE_BLACK;
          w->left->color = RBTREE_BLACK;
          right_rotate(t,x->parent);
          x = t->root;
        }

      }
    }
    x->color = RBTREE_BLACK; // 레드 앤 블랙으로 반복문을 탈출한경우 x의 색을 블랙으로 바꿔줌 x=t->root로 탈출한 경우 그냥 루트노드를 black으로 바꿔줌
  }

  int rbtree_erase(rbtree * t, node_t * z)
  {
    // TODO: implement erase
    node_t* y = z;
    node_t* x;
    color_t deleted_color = y->color;
    //if 와 else if 구문은 자식노드가 하나이거나 없는 경우에 해당하고 여기에 해당하는 경우 삭제되는 색은 삭제되는 노드의 색임
    if(z->left == t->nil){ // 자식노드가 없거나, 오른쪽 자식 하나만 있는 경우
      x = z->right; //자식노드가 없으면 오른쪽 nil노드로 대체, 오른쪽 자식이 있으면 오른쪽 자식으로 대체
      rb_transplant(t,z,z->right); 
    }
    else if(z->right == t->nil){ // 오른쪽 자식만 닐노드이고 왼쪽 자식은 있는 경우
      x = z->left; // 위에 if문에서 걸러졌기 떄문에 이경우는 왼쪽 자식이 무조건 있고 오른쪽 자식은 없는 경우이므로 오른쪽 자식으로 대체
      rb_transplant(t,z,z->left);
    }
    else{  //자식이 둘이상인 경우
      y = tree_minimum(t,z->right); // successor구하기
      deleted_color = y->color; //삭제되는 색은 succesoor의 색
      x = y->right; // 왜 right? left는?? -> y는 z의 succesor이므로 y의 left가 존재할 수가 없음 왜냐면 left는 항상 작은 숫자가 오므로 y의 left가 존재한다면 그 친구가 successor여야했기 때문! 따라서 x는 y의 오른쪽 자식을 넣어주고 오른쪽 자식은 닐노드거나 아니거나 상관 없음!
      if(y->parent == z){ //z가 y의 부모면 --> 대체할 노드가 삭제될 노드의 부모이면
        x->parent = y; // x의 부모를 y로 설정 및 y의 정보를 저장하는 역할을 한다고 볼 수도 있음.
      }
      else{
        rb_transplant(t,y,y->right); // 아니면 삭제된 위치로 옮겨줘야하기 때문에 y의 기존자리에 y의 오른쪽 자식(x)로 대체해줌
        y->right = z->right; // y의 기존 오른쪽 자식인 x를 버리고 새로운 오른쪽자식인 삭제하려는 노드(z)의 오른쪽 자식을 입양--> 현재 삭제되는 노드의 오른쪽 서브트리를 y(대체하는)노드의 오른쪽 서브트리로 옮겨주는 작업 끝냄 
        y->right->parent = y; // 새로 입양한 z의 오른쪽 서브트리들의 부모를 y로 바꿔주어야함 -> 이제 z자리에 y만 옮겨주면 됌.
      }
      rb_transplant(t,z,y); // z 자리로 y를 이동
      y->left = z->left; // y의 왼쪽자식을 버리고 z의 왼쪽 자식 입양
      y->left->parent = y; // z의 부모를 y로 못박기
      y->color = z->color; // y의 색 또한 기존의 z로 바꿔줌
    }
    if(deleted_color == RBTREE_BLACK){
      erase_fixup(t,x);
    }
    
    return 0;
  }

  void inorder_to_array(node_t *node, node_t *nil, key_t *arr, int *index, const size_t n) {
    if (node == nil || *index >= n) return; // NIL 노드에 도달하거나 배열 크기를 초과하면 반환
    inorder_to_array(node->left, nil, arr, index, n);  // 왼쪽 자식 노드 순회
    arr[(*index)++] = node->key; 
    inorder_to_array(node->right, nil, arr, index, n); // 오른쪽 자식 노드 순회
  }

  int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n){
    //if (t == NULL || arr == NULL) return -1; // 오류 처리
    int index = 0; // 배열 인덱스 초기화
    inorder_to_array(t->root, t->nil, arr, &index, n); // 중위 순회를 이용해 배열에 노드 저장
    return 0; // 완료
  }




// 트리를 출력하는 함수
void print_rbtree(rbtree *t, node_t *node, int space)
{
  if (node == t->nil)
    return;

  space += 10;
  print_rbtree(t, node->right, space);

  printf("\n");
  for (int i = 10; i < space; i++)
    printf(" ");
  printf("%d(%s)\n", node->key, node->color == RBTREE_RED ? "R" : "B");

  print_rbtree(t, node->left, space);
}

// int main()
// {
//   rbtree *t = new_rbtree(); // 레드-블랙 트리 생성 함수
//   int key;

//   printf("노드를 삽입하려면 키 값을 입력하세요 (음수를 입력하면 종료):\n");
//   while (scanf("%d", &key) && key >= 0)
//   {
//     rbtree_insert(t, key);
//     print_rbtree(t, t->root, 0);
//   }

//   printf("노드를 지우려면 키 값을 입력하세요 (음스를 입력하면 종료):\n");
//   while (scanf("%d", &key) && key >= 0)
//   {
//     node_t* node = rbtree_find(t,key);
//     rbtree_erase(t, node);
//     print_rbtree(t, t->root, 0);
//   }

//   // 트리 메모리 해제
//   delete_rbtree(t);

//   return 0;
// }