#include "cv.h" 
#include "highgui.h" 

#include <gtk/gtk.h>
#include <glib-object.h>

#include <stdio.h>

#include "filtros_asm/diff_asm.c"
#include "filtros_asm/filtro_reemplazo_fondo_asm.c"
#include "filtros_asm/contarblancos_asm.h"

#include "filtros_c/juego_c.c"



#define TOP_MARGIN 110


typedef IplImage * (aplicar_filtro) (IplImage *src, void *param);


void update_prev_frame();

GtkWidget * toleranciaSlider;
GtkWidget * pxBlancosSlider;

void* dobleParam[2];
void* tripleParam[3];

gint t = 0;
CvCapture* capture;
aplicar_filtro *filtro = NULL;
IplImage * latestFrame = NULL;
IplImage ** previousFrame = NULL;
IplImage * firstFrame = NULL;

char previousFrameInUse = FALSE;

int width;
int height;

int tolerancia;
int pxBlancosPorcent;

void * filtro_param = NULL;


void reinicializar_filtros() {
	previousFrameInUse = FALSE;
	
	if (firstFrame != NULL) {
		cvReleaseImage(&firstFrame);
	}
}


void aplicar_reemplazar_fondo() {
	dobleParam[0] = &firstFrame;
	dobleParam[1] = tolerancia;
	filtro_param = &dobleParam;
	
    filtro = aplicar_filtro_reemplazar_fondo;
}

void update_prev_frame() {
	IplImage * eliminar = previousFrame;
	previousFrame = cvCloneImage( latestFrame );
	cvReleaseImage(&eliminar);
}


void foto_fn( GtkWidget *widget, gpointer data ) {
    cvSaveImage("images/foto.jpg",latestFrame,0);
}
void diff_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	previousFrameInUse = TRUE;
	
	tripleParam[0] = &previousFrame;
	tripleParam[1] = tolerancia;
	tripleParam[2] = pxBlancosPorcent;
	filtro_param = &tripleParam;

    filtro = aplicar_filtro_diff;
}
void reemplazarFondo_fn( GtkWidget *widget, gpointer data ) {
	reinicializar_filtros();
	
	firstFrame = cvCloneImage(latestFrame);
	
	aplicar_reemplazar_fondo();
}


void juego_fn( GtkWidget *widget, gpointer data ) {

    //cvSaveImage("images/image.jpg",latestFrame,0);

    reinicializar_filtros();
    previousFrameInUse = TRUE;

    dobleParam[0] = &previousFrame;
    dobleParam[1] = tolerancia;
    filtro_param = &dobleParam;

    filtro = aplicar_filtro_juego;
}
//+++++++++++++++++LIMPIAR
void limpiar_filtros_fn( GtkWidget *widget, gpointer data ) {
	filtro_param = NULL;
	filtro = NULL;
	reinicializar_filtros();
}

static gboolean time_handler( GtkWidget *widget ) {
  return TRUE;
}

gboolean expose_event_callback( GtkWidget *widget, GdkEventExpose *event, gpointer data) {
	//printf("callback.\n");

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
		frame_procesado = filtro(latestFrame, filtro_param);
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
	return TRUE;
}

void tolerancia_cambiado_fn(GtkWidget *widget, gpointer data) {
	tolerancia = (double)gtk_range_get_value((GtkRange*)widget);

	if (filtro == aplicar_filtro_diff) {
		diff_fn(NULL, NULL);
	} else if (filtro == aplicar_filtro_reemplazar_fondo) {
		aplicar_reemplazar_fondo();
	}
}

void pxBlancos_cambiado_fn(GtkWidget *widget, gpointer data) {
	pxBlancosPorcent = (double)gtk_range_get_value((GtkRange*)widget);

	if (filtro == aplicar_filtro_diff) {
		diff_fn(NULL, NULL);
	}
}

void agregar_tolerancia(GtkWidget *container) {
	GtkWidget * toleranciaContainer;

	toleranciaContainer = gtk_hbox_new(FALSE, 1);

	gtk_scale_set_draw_value((GtkScale*)toleranciaSlider, FALSE);

	toleranciaSlider = gtk_hscale_new_with_range(0, 100, 1);
	gtk_scale_set_draw_value((GtkScale*)toleranciaSlider, FALSE);

	gtk_container_add(GTK_CONTAINER(container), toleranciaContainer);

	gtk_box_pack_start((GtkBox*)toleranciaContainer, toleranciaSlider, TRUE, TRUE, 1);
	g_signal_connect(toleranciaSlider, "value_changed", G_CALLBACK(tolerancia_cambiado_fn), NULL);
	
	gtk_range_set_value((GtkRange*)toleranciaSlider, 15);
}

void agregar_pxBlancos(GtkWidget *container) {
	GtkWidget * pxBlancosContainer;

	pxBlancosContainer = gtk_hbox_new(FALSE, 1);

	gtk_scale_set_draw_value((GtkScale*)pxBlancosSlider, FALSE);

	pxBlancosSlider = gtk_hscale_new_with_range(0, 100, 1);
	gtk_scale_set_draw_value((GtkScale*)pxBlancosSlider, FALSE);

	gtk_container_add(GTK_CONTAINER(container), pxBlancosContainer);

	gtk_box_pack_start((GtkBox*)pxBlancosContainer, pxBlancosSlider, TRUE, TRUE, 1);
	g_signal_connect(pxBlancosSlider, "value_changed", G_CALLBACK(pxBlancos_cambiado_fn), NULL);
	
	gtk_range_set_value((GtkRange*)pxBlancosSlider, 9);
}

void agregar_botones(GtkWidget *container) {
    GtkWidget *hbox_others;
	
    GtkWidget *others_label;
	
	//botones
    GtkWidget *diff_btn;
    GtkWidget *reemplazarFondo_btn;
    GtkWidget *juego_btn;
    GtkWidget *foto_btn;
	
	GtkWidget *limpiar_filtros_btn;
	
    hbox_others = gtk_hbox_new(TRUE, 1);
    gtk_container_add(GTK_CONTAINER(container), hbox_others);
    gtk_widget_set_size_request(hbox_others , width, TOP_MARGIN/2);
	
    others_label = gtk_label_new("Aplicaciones:");
	
	limpiar_filtros_btn = gtk_button_new_with_label("X");
    gtk_container_add(GTK_CONTAINER(container), limpiar_filtros_btn);
	
    gtk_container_add(GTK_CONTAINER(hbox_others), others_label);
	//OTROS
	
    foto_btn = gtk_button_new_with_label("Foto");
    gtk_container_add(GTK_CONTAINER(hbox_others), foto_btn);

    diff_btn = gtk_button_new_with_label("Detector Movimientos");
    gtk_container_add(GTK_CONTAINER(hbox_others), diff_btn);

    reemplazarFondo_btn = gtk_button_new_with_label("Reemplazar Fondo");
    gtk_container_add(GTK_CONTAINER(hbox_others), reemplazarFondo_btn);
    
    juego_btn = gtk_button_new_with_label("Juego");
    gtk_container_add(GTK_CONTAINER(hbox_others), juego_btn);

    agregar_tolerancia(container);
    agregar_pxBlancos(container);

	//eventos
	

	//--------SIGNALS C
    g_signal_connect(foto_btn, "clicked",  G_CALLBACK(foto_fn), NULL);
    g_signal_connect(diff_btn, "clicked",  G_CALLBACK(diff_fn), NULL);
    g_signal_connect(reemplazarFondo_btn, "clicked",  G_CALLBACK(reemplazarFondo_fn), NULL);
    g_signal_connect(juego_btn, "clicked",  G_CALLBACK(juego_fn), NULL);
    g_signal_connect(limpiar_filtros_btn, "clicked",  G_CALLBACK(limpiar_filtros_fn), NULL);

}

void crearImagenDeFondo(int w, int h) {
	IplImage * original = cvLoadImage("images/imagen_original.jpg", CV_LOAD_IMAGE_COLOR);	
	IplImage * fondo = cvCreateImage(cvSize(w, h), original->depth, original->nChannels);
	cvResize(original, fondo, CV_INTER_LINEAR);	
	cvSaveImage("images/fondo.jpg", fondo, 0);
}

int main(int argc, char** argv) {

	GtkWidget *window;
	GtkWidget *drawing_area;
	GtkWidget *vbox;

	gtk_init (&argc, &argv);
	capture = cvCaptureFromCAM( CV_CAP_ANY );
	
    latestFrame = cvQueryFrame( capture );
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
	g_timeout_add( 100, ( GSourceFunc )time_handler, ( gpointer )drawing_area );
	
	crearImagenDeFondo(width, height);
	
	reinicializar_filtros();
	
	gtk_widget_show_all(window);
	gtk_main();
	
	cvReleaseCapture( &capture );
	
	return 0;
 }
