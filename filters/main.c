#include "cv.h" 
#include "highgui.h" 

#include <gtk/gtk.h>
#include <glib-object.h>

#include <stdio.h>
#include <sys/time.h>
#include <string.h>

#include "filtros_asm/monocromatizar_asm.c"
#include "filtros_asm/suavizar_asm.c"
#include "filtros_asm/invertir_asm.c"
#include "filtros_asm/separar_canales_asm.c"
#include "filtros_asm/brillo_asm.c"
#include "filtros_asm/contraste_asm.c"
#include "filtros_asm/sharpness_asm.c"
#include "filtros_asm/noise_asm.c"
#include "filtros_asm/monocromatizar_bin_asm.c"
#include "filtros_asm/borde_lap_asm.c"

#include "filtros_c/sharpness_c.c"
#include "filtros_c/suavizar_c_rgb.c"
#include "filtros_c/separar_canales_c.h"
#include "filtros_c/monocromatizar_inf_c.c"
#include "filtros_c/monocromatizar_bin_c.c"
#include "filtros_c/borde_lap_c.h"
#include "filtros_c/brillo_c.h"
#include "filtros_c/contraste_c.h"
#include "filtros_c/noise_c.c"
#include "filtros_c/invertir_c.h"


#define TOP_MARGIN 120
#define APLICACION_FILTRO_TESTS 10

typedef IplImage * (aplicar_filtro) (IplImage *src, void *param);


void resetear_brillo_contraste();
void update_prev_frame();
void resetear_brillo();
void resetear_contraste();

gint t = 0;
CvCapture* capture;
aplicar_filtro *filtro = NULL;
IplImage * latestFrame = NULL;
IplImage ** previousFrame = NULL;

char previousFrameInUse = FALSE;

GtkWidget * brilloSlider;
GtkWidget * contrasteSlider;
GtkWidget * brilloCRadio;
GtkWidget * contrasteCRadio;

int width;
int height;

int brillo;
int contraste;

void * filtro_param = NULL;

int numFrame = 0;
long long tiempoComienzoFiltro = 0;
typedef void (filtro_f_t) (void*, void*);
int indexFiltroActual = 0;
char testsCorriendo = FALSE;
typedef enum {C, ASM} TipoImpl;
TipoImpl tipoImplementacion = C;
int APLICACION_FILTRO_N_VECES = 1;
long long MILISEG_TEST_DURACION = 1000 * 10;	//10 segundos




void reinicializar_filtros() {
	previousFrameInUse = FALSE;
	gtk_toggle_button_set_active((GtkToggleButton*)brilloCRadio, TRUE);
	gtk_toggle_button_set_active((GtkToggleButton*)contrasteCRadio, TRUE);
	resetear_brillo_contraste();
}

void update_prev_frame() {
	IplImage * eliminar = previousFrame;
	previousFrame = cvCloneImage( latestFrame );
	cvReleaseImage(&eliminar);
}


//signal callbacks
//------ASM
void monocromatizar_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	filtro = aplicar_filtro_monocromatizar;
}

void suavizar_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	filtro = aplicar_filtro_suavizar;
}

void invertir_fn( GtkWidget *widget, gpointer data ) {
	filtro = aplicar_filtro_invertir;
}

void bordes_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	filtro = aplicar_filtro_borde_asm;
}

void monocromatizar_bin_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	filtro = aplicar_filtro_monocromatizar_bin_asm;
}

void ruido_fn( GtkWidget *widget, gpointer data ) {
    reinicializar_filtros();
    previousFrameInUse = TRUE;
    update_prev_frame();
    filtro_param = &previousFrame;

    filtro = aplicar_filtro_noise_asm;
}

void red_fn( GtkWidget *widget, gpointer data ) {
    filtro = aplicar_filtro_red;
}

void green_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	filtro = aplicar_filtro_green;
}

void blue_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
    filtro = aplicar_filtro_blue;
}

void sharpness_asm_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	filtro = aplicar_filtro_sharpness_asm;
}

//------   C

void monocromatizar_c_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	filtro = aplicar_filtro_monocromatizar_c;
}

void suavizar_c_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	filtro = aplicar_filtro_suavizado_c_rgb;
}

void invertir_c_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	filtro = aplicar_filtro_invertir_c;
}

void sharpness_c_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
    filtro = aplicar_filtro_sharpness;
}

void bordes_c_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
    filtro = aplicar_filtro_borde_c;
}

void monocromatizar_bin_c_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
    filtro = aplicar_filtro_monocromatizar_bin_c;
}

void ruido_c_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	previousFrameInUse = TRUE;
	update_prev_frame();
	filtro_param = &previousFrame;

	filtro = aplicar_filtro_noise;
}

void red_c_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
    filtro = aplicar_filtro_red_c;
}

void green_c_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
    filtro = aplicar_filtro_green_c;
}

void blue_c_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
    filtro = aplicar_filtro_blue_c;
}

//+++++++++++++++++LIMPIAR
void limpiar_filtros_fn( GtkWidget *widget, gpointer data ) {
	filtro_param = NULL;
	filtro = NULL;
	reinicializar_filtros();
}

//se usan para tests
void brillo_fn(void* sender, void* data) {
	filtro_param = &brillo;
	filtro = aplicar_filtro_brillo_asm;
}

void brillo_c_fn(void* sender, void* data) {
	filtro_param = &brillo;
	filtro = aplicar_filtro_brillo_c;
}

void contraste_fn(void* sender, void* data) {
	filtro_param = &contraste;
	filtro = aplicar_filtro_contraste_asm;
}

void contraste_c_fn(void* sender, void* data) {
	filtro_param = &contraste;
	filtro = aplicar_filtro_contraste_c;
}



void aplicar_brillo() {
	previousFrameInUse = FALSE;
	resetear_contraste();
	
	filtro_param = &brillo;
	
	gboolean c = gtk_toggle_button_get_active((GtkToggleButton*)brilloCRadio);
	
	if (c) {
		filtro = aplicar_filtro_brillo_c;
	} else {
		filtro = NULL;
		filtro = aplicar_filtro_brillo_asm;
	}
}

void aplicar_contraste() {
	previousFrameInUse = FALSE;
	resetear_brillo();
	
	filtro_param = &contraste;
	
	gboolean c = gtk_toggle_button_get_active((GtkToggleButton*)contrasteCRadio);
	
	if (c) {
		filtro = aplicar_filtro_contraste_c;
	} else {
		filtro = NULL;
		filtro = aplicar_filtro_contraste_asm;
	}
}

void brillo_cambiado_fn(GtkWidget *widget, gpointer data) {
	brillo = (double)gtk_range_get_value((GtkRange*)widget);
	aplicar_brillo();
}

void contraste_cambiado_fn(GtkWidget *widget, gpointer data) {
	contraste = (double)gtk_range_get_value((GtkRange*)widget);
	aplicar_contraste();
}

void brilloRadio_cambiado_fn(GtkWidget *widget, gpointer data) {
	brillo = gtk_range_get_value((GtkRange*)brilloSlider);
	aplicar_brillo();
}

void contrasteRadio_cambiado_fn(GtkWidget *widget, gpointer data) {
	contraste = gtk_range_get_value((GtkRange*)contrasteSlider);
	aplicar_contraste();
}


static gboolean time_handler( GtkWidget *widget ) {
  return TRUE;
}

//////////////////////////////
/// Definicion de los filtros
//////////////////////////////


char* filtros[] = {
	"monocromatizar", 
	"monocromatizar binario", 
	"suavizar", 
	"sharpness",
	"invertir", 
	"deteccion bordes", 
	"eliminar ruido", 
	"extraer canal rojo",
	"extraer canal verde", 
	"extraer canal azul", 
	"contraste", 
	"brillo"
};
	
void (*filtros_asm_fn[]) (void*, void*) = {
	(filtro_f_t*) monocromatizar_fn,
	(filtro_f_t*) monocromatizar_bin_fn,
	(filtro_f_t*) suavizar_fn,
	(filtro_f_t*) sharpness_asm_fn,
	(filtro_f_t*) invertir_fn,
	(filtro_f_t*) bordes_fn,
	(filtro_f_t*) ruido_fn,
	(filtro_f_t*) red_fn,
	(filtro_f_t*) green_fn,
	(filtro_f_t*) blue_fn,
	(filtro_f_t*) contraste_fn,
	(filtro_f_t*) brillo_fn,
};

void (*filtros_c_fn[]) (void*, void*) = {
	(filtro_f_t*) monocromatizar_c_fn,
	(filtro_f_t*) monocromatizar_bin_c_fn,
	(filtro_f_t*) suavizar_c_fn,
	(filtro_f_t*) sharpness_c_fn,
	(filtro_f_t*) invertir_c_fn,
	(filtro_f_t*) bordes_c_fn,
	(filtro_f_t*) ruido_c_fn,
	(filtro_f_t*) red_c_fn,
	(filtro_f_t*) green_c_fn,
	(filtro_f_t*) blue_c_fn,
	(filtro_f_t*) contraste_c_fn,
	(filtro_f_t*) brillo_c_fn,
};


long long get_timestamp_mili() {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long long timestamp = tp.tv_sec*1000 + tp.tv_usec / 1000;	//milisegundos
	return timestamp;
}

int obtener_fps() {
	long long timestamp = get_timestamp_mili();
	int fps = (int)(numFrame * 1000 / (timestamp - tiempoComienzoFiltro));
	return fps;
}

void ejecutar_filtro(int index, TipoImpl implementacion) {
	filtro_f_t *filtro_f;
	
	filtro_f = filtros_asm_fn[index];
	if (implementacion == C) {
		filtro_f = filtros_c_fn[index];
	}
	
	filtro_f(NULL, NULL);
}

void imprimir_resultado() {
	int fps = obtener_fps();
	char impl[4];
	if (tipoImplementacion == C) strcpy(impl, "c");
	else strcpy(impl, "asm");
	
	long long timestamp = get_timestamp_mili();
		
	printf("\"%s %s\" %d numFrame: %d - duracion: %lld ms\n", filtros[indexFiltroActual], impl, fps, numFrame, (timestamp - tiempoComienzoFiltro));
}

void siguiente_test() {
	if (!testsCorriendo) {
		return;
	}

	if (indexFiltroActual == 11 && tipoImplementacion == ASM) {
		imprimir_resultado();
		testsCorriendo = FALSE;
		APLICACION_FILTRO_N_VECES = 1;
		printf("finalizados los tests\n");
		return;
	}
	
	imprimir_resultado();
	
	if (tipoImplementacion == C) {
		tipoImplementacion = ASM;
	} else {
		tipoImplementacion = C;
		indexFiltroActual++;
	}
	
	tiempoComienzoFiltro = get_timestamp_mili();
	numFrame = 0;
	
	ejecutar_filtro(indexFiltroActual, tipoImplementacion);
}

void hacer_tests() {
	testsCorriendo = TRUE;
	APLICACION_FILTRO_N_VECES = APLICACION_FILTRO_TESTS;
	tipoImplementacion = C;
	indexFiltroActual = 0;
	
	numFrame = 0;
	tiempoComienzoFiltro = get_timestamp_mili();

	ejecutar_filtro(indexFiltroActual, tipoImplementacion);
}

void contarFrame() {
	numFrame++;	
	
	long long timestamp = get_timestamp_mili();
	if (timestamp - tiempoComienzoFiltro >= MILISEG_TEST_DURACION) {
		siguiente_test();
	}
}

gboolean expose_event_callback( GtkWidget *widget, GdkEventExpose *event, gpointer data) {
	IplImage *resultado;
	IplImage *frame_procesado;
	
	GdkPixbuf* pix;
	
	
	gtk_widget_queue_draw( GTK_WIDGET( widget ));
	
    if (previousFrameInUse) {
    	update_prev_frame();
    }
    
    latestFrame = cvQueryFrame( capture );
    frame_procesado = NULL;

	if (filtro != NULL) {
		for (int i = 0; i < APLICACION_FILTRO_N_VECES; i++) {
        	frame_procesado = filtro(latestFrame, filtro_param);
        	if (i < APLICACION_FILTRO_N_VECES -1) {
        		cvReleaseImage(&frame_procesado);
        	}
        }
	} else {
        frame_procesado = cvCloneImage(latestFrame);
	}
	
	resultado = cvCreateImage(cvGetSize(frame_procesado), IPL_DEPTH_8U, 3);
	
	if (frame_procesado->nChannels == 1) {
		cvCvtColor( frame_procesado, resultado, CV_GRAY2RGB);
	} else {
		cvCvtColor( frame_procesado, resultado, CV_BGR2RGB );
	}
   
   
	pix = g_object_ref( gdk_pixbuf_new_from_data(( guchar* )resultado->imageData,
                                             GDK_COLORSPACE_RGB,
                                             FALSE,
                                             resultado->depth,
                                             resultado->width,
                                             resultado->height,
                                             ( resultado->widthStep ),
                                             NULL, NULL) );
	gdk_draw_pixbuf( widget->window,
                    widget->style->fg_gc[ GTK_WIDGET_STATE( widget )],
                    pix,
                    0, 0, 0, 0,
                    resultado->width,
                    resultado->height,
                    GDK_RGB_DITHER_MAX,
                    0, 0);
                    
	g_object_unref(pix);
                    
    //liberar memoria
    cvReleaseImage(&frame_procesado);
    cvReleaseImage(&resultado);
    
    contarFrame();
    
	return TRUE;
}


void resetear_brillo_contraste() {
	resetear_brillo();
	resetear_contraste();
}

void resetear_brillo() {
	brillo = 50;
	gtk_range_set_value((GtkRange*)brilloSlider, brillo);
}

void resetear_contraste() {
	contraste = 0;
	gtk_range_set_value((GtkRange*)contrasteSlider, contraste);
}

void configurar_sliders(GtkWidget *container) {
	GtkWidget * brilloLabel;
	GtkWidget * brilloContainer;
	GtkWidget * brilloAsmRadio;
	
	brilloSlider = gtk_hscale_new_with_range(0, 100, 1);
	gtk_scale_set_draw_value((GtkScale*)brilloSlider, FALSE);
	
	brilloLabel = gtk_label_new("Brillo:");
	brilloContainer = gtk_hbox_new(FALSE, 1);
	brilloCRadio = gtk_radio_button_new_with_label (NULL, "C");
	brilloAsmRadio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(brilloCRadio), "ASM");
	
	
	gtk_box_pack_start((GtkBox*)brilloContainer, brilloLabel, FALSE, TRUE, 0);
	gtk_box_pack_start((GtkBox*)brilloContainer, brilloSlider, TRUE, TRUE, 1);
	gtk_box_pack_start((GtkBox*)brilloContainer, brilloCRadio, FALSE, TRUE, 2);
	gtk_box_pack_start((GtkBox*)brilloContainer, brilloAsmRadio, FALSE, TRUE, 3);
	gtk_container_add(GTK_CONTAINER(container), brilloContainer);

	g_signal_connect(brilloSlider, "value_changed", G_CALLBACK(brillo_cambiado_fn), NULL);
	
	
	GtkWidget * contrasteLabel;
	GtkWidget * contrasteContainer;
	GtkWidget * contrasteAsmRadio;
	
	contrasteSlider = gtk_hscale_new_with_range(-100, 100, 1);
	gtk_scale_set_draw_value((GtkScale*)contrasteSlider, FALSE);
	
	contrasteLabel = gtk_label_new("Contraste:");
	contrasteContainer = gtk_hbox_new(FALSE, 1);
	contrasteCRadio = gtk_radio_button_new_with_label (NULL, "C");
	contrasteAsmRadio = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(contrasteCRadio), "ASM");

	
	gtk_box_pack_start((GtkBox*)contrasteContainer, contrasteLabel, FALSE, TRUE, 0);
	gtk_box_pack_start((GtkBox*)contrasteContainer, contrasteSlider, TRUE, TRUE, 1);
	gtk_box_pack_start((GtkBox*)contrasteContainer, contrasteCRadio, FALSE, TRUE, 2);
	gtk_box_pack_start((GtkBox*)contrasteContainer, contrasteAsmRadio, FALSE, TRUE, 3);
	gtk_container_add(GTK_CONTAINER(container), contrasteContainer);
	
	g_signal_connect(contrasteSlider, "value_changed", G_CALLBACK(contraste_cambiado_fn), NULL);
	
	g_signal_connect(brilloCRadio, "toggled", G_CALLBACK(brilloRadio_cambiado_fn), NULL);
	g_signal_connect(brilloAsmRadio, "toggled", G_CALLBACK(brilloRadio_cambiado_fn), NULL);
	g_signal_connect(contrasteCRadio, "toggled", G_CALLBACK(contrasteRadio_cambiado_fn), NULL);
	g_signal_connect(contrasteAsmRadio, "toggled", G_CALLBACK(contrasteRadio_cambiado_fn), NULL);

	resetear_brillo_contraste();
}

void agregar_botones(GtkWidget *container) {
	GtkWidget *hbox_asm;
	GtkWidget *hbox_c;
	
	GtkWidget *asm_label;
	GtkWidget *c_label;
	
	GtkWidget *monocromatizar_btn;
	GtkWidget *suavizar_btn;
	GtkWidget *invertir_btn;
	
    GtkWidget *bordes_btn;
	GtkWidget *monocromatizar_bin_btn;
	GtkWidget *ruido_btn;
	GtkWidget *red_btn;
	GtkWidget *green_btn;
	GtkWidget *blue_btn;
	
	GtkWidget *sharpness_asm_btn;

	//botones C
	GtkWidget *sharpness_c_btn;
    GtkWidget *suavizar_c_btn;
    GtkWidget *red_c_btn;
    GtkWidget *green_c_btn;
    GtkWidget *blue_c_btn;
    GtkWidget *monocromatizar_c_btn;
    GtkWidget *monocromatizar_bin_c_btn;
    GtkWidget *bordes_c_btn;
    GtkWidget *ruido_c_btn;
    GtkWidget *invertir_c_btn;
	GtkWidget *limpiar_filtros_btn;
	
	
	hbox_asm = gtk_hbox_new(TRUE, 1);
	gtk_container_add(GTK_CONTAINER(container), hbox_asm);
	gtk_widget_set_size_request(hbox_asm , width, 20);
	
	hbox_c = gtk_hbox_new(TRUE, 1);
	gtk_container_add(GTK_CONTAINER(container), hbox_c);
	gtk_widget_set_size_request(hbox_c , width, 20);
		
	
    asm_label = gtk_label_new("ASM:");
	c_label = gtk_label_new("C:");
	
	limpiar_filtros_btn = gtk_button_new_with_label("X");
    gtk_container_add(GTK_CONTAINER(container), limpiar_filtros_btn);
	
	gtk_container_add(GTK_CONTAINER(hbox_asm), asm_label);
	gtk_container_add(GTK_CONTAINER(hbox_c), c_label);
	
	configurar_sliders(container);
	
	//BOTONES ASM
	
	monocromatizar_btn = gtk_button_new_with_label("Monocromatizar");
	gtk_container_add(GTK_CONTAINER(hbox_asm), monocromatizar_btn);

	monocromatizar_bin_btn = gtk_button_new_with_label("Mono Bin.");
	gtk_container_add(GTK_CONTAINER(hbox_asm), monocromatizar_bin_btn);
	
	suavizar_btn = gtk_button_new_with_label("Suavizar");
	gtk_container_add(GTK_CONTAINER(hbox_asm), suavizar_btn);
	
	sharpness_asm_btn = gtk_button_new_with_label("Sharpness");
	gtk_container_add(GTK_CONTAINER(hbox_asm), sharpness_asm_btn);
	
	invertir_btn = gtk_button_new_with_label("Invertir");
	gtk_container_add(GTK_CONTAINER(hbox_asm), invertir_btn);

	bordes_btn = gtk_button_new_with_label("Bordes");
	gtk_container_add(GTK_CONTAINER(hbox_asm), bordes_btn);

	ruido_btn = gtk_button_new_with_label("-Ruido");
	gtk_container_add(GTK_CONTAINER(hbox_asm), ruido_btn);
	
	red_btn = gtk_button_new_with_label("Rojo");
	gtk_container_add(GTK_CONTAINER(hbox_asm), red_btn);
	
	green_btn = gtk_button_new_with_label("Verde");
	gtk_container_add(GTK_CONTAINER(hbox_asm), green_btn);
	
	blue_btn = gtk_button_new_with_label("Azul");
	gtk_container_add(GTK_CONTAINER(hbox_asm), blue_btn);
	
	//BOTONES C	
    monocromatizar_c_btn = gtk_button_new_with_label("Monocromatizar");
    gtk_container_add(GTK_CONTAINER(hbox_c), monocromatizar_c_btn);
    
    monocromatizar_bin_c_btn = gtk_button_new_with_label("Mono Bin.");
    gtk_container_add(GTK_CONTAINER(hbox_c), monocromatizar_bin_c_btn);
    
    suavizar_c_btn = gtk_button_new_with_label("Suavizar");
    gtk_container_add(GTK_CONTAINER(hbox_c), suavizar_c_btn);
    
    sharpness_c_btn = gtk_button_new_with_label("Sharpness");
    gtk_container_add(GTK_CONTAINER(hbox_c), sharpness_c_btn);

    invertir_c_btn = gtk_button_new_with_label("Invertir");
    gtk_container_add(GTK_CONTAINER(hbox_c), invertir_c_btn);

    bordes_c_btn = gtk_button_new_with_label("Bordes");
    gtk_container_add(GTK_CONTAINER(hbox_c), bordes_c_btn);
    
    ruido_c_btn = gtk_button_new_with_label("-Ruido");
    gtk_container_add(GTK_CONTAINER(hbox_c), ruido_c_btn);

    red_c_btn = gtk_button_new_with_label("Rojo");
    gtk_container_add(GTK_CONTAINER(hbox_c), red_c_btn);
    
    green_c_btn = gtk_button_new_with_label("Verde");
    gtk_container_add(GTK_CONTAINER(hbox_c), green_c_btn);
    
    blue_c_btn = gtk_button_new_with_label("Azul");
    gtk_container_add(GTK_CONTAINER(hbox_c), blue_c_btn);

	
	//eventos
	
	//SIGNALS ASM
	
	g_signal_connect(monocromatizar_btn, "clicked",  G_CALLBACK(monocromatizar_fn), NULL);
	g_signal_connect(suavizar_btn, "clicked",  G_CALLBACK(suavizar_fn), NULL);
    g_signal_connect(invertir_btn, "clicked",  G_CALLBACK(invertir_fn), NULL);
    g_signal_connect(limpiar_filtros_btn, "clicked",  G_CALLBACK(limpiar_filtros_fn), NULL);
	
	
    g_signal_connect(sharpness_asm_btn, "clicked",  G_CALLBACK(sharpness_asm_fn), NULL);
    
    g_signal_connect(bordes_btn, "clicked",  G_CALLBACK(bordes_fn), NULL);
    g_signal_connect(monocromatizar_bin_btn, "clicked",  G_CALLBACK(monocromatizar_bin_fn), NULL);
    g_signal_connect(ruido_btn, "clicked",  G_CALLBACK(ruido_fn), NULL);
    g_signal_connect(red_btn, "clicked",  G_CALLBACK(red_fn), NULL);
    g_signal_connect(green_btn, "clicked",  G_CALLBACK(green_fn), NULL);
    g_signal_connect(blue_btn, "clicked",  G_CALLBACK(blue_fn), NULL);

	//--------SIGNALS C
    g_signal_connect(sharpness_c_btn, "clicked",  G_CALLBACK(sharpness_c_fn), NULL);
    g_signal_connect(suavizar_c_btn, "clicked",  G_CALLBACK(suavizar_c_fn), NULL);
    g_signal_connect(red_c_btn, "clicked",  G_CALLBACK(red_c_fn), NULL);
    g_signal_connect(green_c_btn, "clicked",  G_CALLBACK(green_c_fn), NULL);
    g_signal_connect(blue_c_btn, "clicked",  G_CALLBACK(blue_c_fn), NULL);
    g_signal_connect(monocromatizar_c_btn, "clicked",  G_CALLBACK(monocromatizar_c_fn), NULL);
    g_signal_connect(monocromatizar_bin_c_btn, "clicked",  G_CALLBACK(monocromatizar_bin_c_fn), NULL);
    g_signal_connect(bordes_c_btn, "clicked",  G_CALLBACK(bordes_c_fn), NULL);
    g_signal_connect(ruido_c_btn, "clicked",  G_CALLBACK(ruido_c_fn), NULL);
    g_signal_connect(invertir_c_btn, "clicked",  G_CALLBACK(invertir_c_fn), NULL);
}


int main(int argc, char** argv) {

	GtkWidget *window;
	GtkWidget *drawing_area;
	GtkWidget *vbox;

	gtk_init (&argc, &argv);
	capture = cvCaptureFromCAM( CV_CAP_ANY );
	
    latestFrame = cvQueryFrame( capture );
    cvSaveImage("images/image.jpg",latestFrame,0);
	width = latestFrame->width;
	height = latestFrame->height;
	
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title( GTK_WINDOW( window ), "TP Final - Orga2");
	gtk_widget_set_size_request( window, width, height + TOP_MARGIN);
	g_signal_connect( G_OBJECT( window ), "destroy", G_CALLBACK (gtk_main_quit), NULL);
	
	GtkWidget *frame;
	frame = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), frame);
	
	vbox = gtk_vbox_new(TRUE, 1);
	gtk_fixed_put(GTK_FIXED(frame), vbox, 0, 0);
	gtk_widget_set_size_request(vbox, width, TOP_MARGIN);
	
	agregar_botones(vbox);
	
	drawing_area = gtk_drawing_area_new();
	gtk_widget_set_size_request( drawing_area, width, height );
	gtk_fixed_put(GTK_FIXED(frame), drawing_area, 0, TOP_MARGIN);
	
	g_signal_connect( G_OBJECT( drawing_area), "expose_event", G_CALLBACK (expose_event_callback), NULL);
	g_timeout_add(50, ( GSourceFunc )time_handler, ( gpointer )drawing_area );
	
	reinicializar_filtros();
	
	gtk_widget_show_all(window);
	
	if (argc > 1 && strcmp("test", argv[1]) == 0) {
		hacer_tests();
	}
	
	gtk_main();
	
	cvReleaseCapture( &capture );
	
	return 0;
 }
