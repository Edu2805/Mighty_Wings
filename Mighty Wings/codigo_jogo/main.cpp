#include <allegro.h>
#include <iostream.h>

const int VELOCIDADE_DA_NAVE = 12;

const int TOTAL_DE_INIMIGOS = 10;

struct Objeto{
	int x;
	int y;
	float vspeed;
	float hspeed;
	BITMAP* sprite;
};

//prototipos
void reposicionaInimigo(Objeto& inimigo);

//vari·veis globais
Objeto naveDoJogo;
Objeto tiro;
Objeto background;
Objeto inimigos[ TOTAL_DE_INIMIGOS ];

BITMAP* buffer;

//++++++++++++++++++++++++++++++++++++++++++++++
void inicializaVariaveis(){

	background.sprite = load_bitmap( "background.bmp", NULL );


	naveDoJogo.x = naveDoJogo.y = 400;
	naveDoJogo.vspeed = naveDoJogo.hspeed = 0;
	naveDoJogo.sprite = load_bitmap( "nave.bmp", NULL );

	tiro.x = tiro.y = -50;
	tiro.vspeed = tiro.hspeed = 0;
	tiro.sprite = load_bitmap( "tiro.bmp", NULL );

	BITMAP* spriteDosInimigos = load_bitmap( "naveInimiga.bmp", NULL );
	for ( int i = 0;i < TOTAL_DE_INIMIGOS;i++ ){
		inimigos[i].x = i * 400;
		inimigos[i].y = 500;
		inimigos[i].sprite = spriteDosInimigos;
		inimigos[i].hspeed = -12;
		inimigos[i].vspeed = 0;
	}
	background.x = 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++
void inicializaJogo(){
	srand( time( 0 ) );//inicializa random
	allegro_init();//inicializa allegro
	install_keyboard();//

	set_color_depth( 32 );
	set_gfx_mode( GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0 );

	buffer = create_bitmap( SCREEN_W, SCREEN_H );

	inicializaVariaveis();
}
//++++++++++++++++++++++++++++++++++++++++++++++
bool podeAtirar(){
	return key[KEY_SPACE] && tiro.hspeed == 0;
}
//+++++++++++++++++++++++++++
void atira(){
	tiro.hspeed = 30;
	tiro.x = naveDoJogo.x + naveDoJogo.sprite->w;
	tiro.y = naveDoJogo.y + ( naveDoJogo.sprite->h / 2 );
}
//++++++++++++++++++++++++++++++++++
void trataTiro(){
	if ( podeAtirar() ){
		atira();
	}
}
//++++++++++++++++++++++++++++++++++++++++++++++
void trataMovimentacao(){
	if ( key[KEY_LEFT] || key[KEY_RIGHT] ){
		if ( key[KEY_LEFT] )
			naveDoJogo.hspeed = -VELOCIDADE_DA_NAVE;
		else
			naveDoJogo.hspeed = VELOCIDADE_DA_NAVE;
	}
	else
		naveDoJogo.hspeed = 0;

	if ( key[KEY_UP] || key[KEY_DOWN] ){
		if ( key[KEY_UP] )
			naveDoJogo.vspeed = -5;
		else
			naveDoJogo.vspeed = 5;
	}
	else
		naveDoJogo.vspeed = 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++
void trataEntradasDoTeclado(){
	trataMovimentacao();
	trataTiro();
}
//++++++++++++++++++++++++++++++++++++++++++++++
void atualizaPosicaoDoObjeto( Objeto & objetoQueSeraAtualizado ){
	objetoQueSeraAtualizado.x += objetoQueSeraAtualizado.hspeed;
	objetoQueSeraAtualizado.y += objetoQueSeraAtualizado.vspeed;
}
//++++++++++++++++++++++++++++++++++++++++++++++
void atualizaPosicoesDosObjetos(){

	atualizaPosicaoDoObjeto( naveDoJogo );
	atualizaPosicaoDoObjeto( tiro );

	for ( int i = 0; i < TOTAL_DE_INIMIGOS; i++ ){
		atualizaPosicaoDoObjeto( inimigos[i] );
		if ( inimigos [i].x + inimigos [i].sprite->w <= 0 ){
			inimigos [i].x = 800;
		}
	}
	if ( tiro.x > SCREEN_W )
		tiro.hspeed = 0;
}
//+++++++++++++++++++++++++++++++++
bool deveResetarScrolling(){
	return background.x <= -background.sprite->w;
}
//+++++++++++++++++++++++++++++++++++++++++++
void desenhaObjeto( Objeto objetoQueSeraDesenhado ){
	draw_sprite( buffer, objetoQueSeraDesenhado.sprite, objetoQueSeraDesenhado.x, objetoQueSeraDesenhado.y );
}
//+++++++++++++++++++++++++++++++++++++++++++
void desenhaElementosDoJogo(){
	//desenha background com scrolling
	draw_sprite( buffer, background.sprite, background.x, 0 );
	draw_sprite( buffer, background.sprite, background.x + background.sprite->w, 0 );
	background.x -= 10;
	if ( deveResetarScrolling() )
		background.x = 0;

	//desenha a nave
	desenhaObjeto( naveDoJogo );

// desenha inimigo
	for(int i =0;i < TOTAL_DE_INIMIGOS;i++){
		desenhaObjeto( inimigos[i] );
	}
	//desenha tiro
	desenhaObjeto( tiro );

	draw_sprite( screen, buffer, 0, 0 );
}
//+++++++++++++++++++++++++++
bool inimigoColidiuComTiro(Objeto& umInimigo){
	int tiroRight = tiro.x + tiro.sprite->w;
	int inimigoRight = umInimigo.x + umInimigo.sprite->w;
	int tiroBottom = tiro.y + tiro.sprite->h;
	int inimigoBottom = umInimigo.y + umInimigo.sprite->h;
	bool colidiuEmX = ( tiroRight >= umInimigo.x ) && ( tiro.x <= inimigoRight );
	bool colidiuEmY = ( tiroBottom >= umInimigo.y ) && ( tiro.y <= inimigoBottom );

	if ( colidiuEmX && colidiuEmY ){
		tiro.x = -50;
		tiro.y = -50;

		reposicionaInimigo(umInimigo);
	}
}
//+++++++++++++++++++++++++++
void reposicionaInimigo(Objeto& inimigo){
	inimigo.x = SCREEN_W;
	inimigo.y = rand() % SCREEN_H - inimigo.sprite->h;
}
//+++++++++++++++++++++++++++
void verificaColisoes(){
	//passa em todos os inimigos e vÍ se quem colidiu com o tiro
	//se coliciu esconde o tiro e reseta posiÁ„o do inimgo
	for ( int i=0; i < TOTAL_DE_INIMIGOS; i++){
		if( inimigoColidiuComTiro( inimigos[i] ) ){
			//escondo o tiro
			tiro.x = tiro.y = -50;
			//reseto a posiÁ„o do inimigo
			reposicionaInimigo( inimigos[i] );
		}
	}
}
//++++++++++++++++++++++++++++++++++
int main(){

	inicializaJogo();
	while ( !key[KEY_ESC] ){//game loop
		trataEntradasDoTeclado();
		atualizaPosicoesDosObjetos();
		verificaColisoes();
		desenhaElementosDoJogo();

		rest( 20 );
	}

	allegro_exit();
	return 0;
}
END_OF_MAIN()


