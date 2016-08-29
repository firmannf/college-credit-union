#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include <iomanip>

#define MAX_ANGGOTA 50
#define MAX_SIMPANAN 1000
#define MAX_PINJAMAN 1000
#define SIMPANAN_POKOK 200000
#define SIMPANAN_WAJIB 50000

using namespace std;

/* 
	Deklarasi Variabel
*/
typedef struct {
        int tgl;
        int bln;
        int thn;
}tanggal;

typedef struct{
	char jalan_komplek[51];
	char kelurahan[31];
	char kecamatan[31];
	char kota[31];
	char kode_pos[7];
}alamat;

typedef struct{
	int id_simpanan;
	long int jml_simpanan;
	tanggal tgl_simpanan;
}simpanan;

typedef struct{
	int id_ambil;
	long int jml_ambil;
	tanggal tgl_ambil;
}ambil_simpanan;

typedef struct{
	int id_pinjam;
	double jml_pinjam;
	int jml_angsuran_pinjam; //Berapa kali mengangsur
	double bunga_pinjam;
	double cicilan_pinjam;
	double sisa_pinjam;
	int angsuran_ke_pinjam; // Angsuran ke-1, ke-2, dst
	int status_pinjam; //0 lunas, 1 belum lunas;
	tanggal tgl_pinjam;
}pinjaman;

typedef struct{
	//Biodata Anggota
	int id_anggota;
	char nama_anggota[26];
	alamat alamat_anggota;
	char jk_anggota;
	tanggal tgl_lahir_anggota;
	tanggal tgl_masuk_anggota;
	char notelp_anggota[18];
	
	//Simpanan
	int simpanan_pokok; //Simpanan saat pendaftaran anggota
	simpanan simpanan_wajib[MAX_SIMPANAN]; //Simpanan rutin per bulan
	simpanan simpanan_sukarela[MAX_SIMPANAN]; //Simpanan bebas kapanpun
	ambil_simpanan ambil_simpanan_sukarela[MAX_SIMPANAN]; //Ambil simpanan sukarela
	int total_simpanan_sukarela; //Simpanan sukarela bisa diambil
	
	//Pinjaman
	pinjaman pinjam[MAX_PINJAMAN];
}anggota;

//Var buat kebutuhan anggota
anggota agt[MAX_ANGGOTA];
int indeks_simpanan_wajib[MAX_ANGGOTA];
int indeks_simpanan_sukarela[MAX_ANGGOTA];
int indeks_ambil_sukarela[MAX_ANGGOTA];
int indeks_pinjaman[MAX_ANGGOTA];
int counter_anggota = 0;

//Keuangan koperasi
double uang_koperasi = 0;
long int modal_awal = 50000000;
long int jum_simpanan_pokok = 0;
long int jum_simpanan_wajib = 0;
long int jum_simpanan_sukarela = 0;
long int jum_ambil_sukarela = 0;
double jum_sisa_pinjam = 0;
double jum_bunga = 0;
/* End Deklarasi Variabel */

/* Deklarasi Procedure */
void keuangan_koperasi();
void login();
/* End Deklarasi Procedure */


/*
  	Ganti warna text
	https://msdn.microsoft.com/en-us/library/windows/desktop/ms686047(v=vs.85).aspx
*/
void set_text_color(unsigned short color){
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon,color);
}

/*
	Procedure untuk menghasilkan tanggal sekarang
*/
void date_now(int *tgl, int *bln, int *thn){
	time_t theTime = time(NULL);
	struct tm *aTime = localtime(&theTime);
	*tgl = aTime->tm_mday;
	*bln = aTime->tm_mon + 1;
	*thn = aTime->tm_year + 1900;
}

/*
	Procedure untuk mencari anggota berdasar id
*/
void cari_id_anggota(int cari_id, bool *ketemu, int *posisi_ketemu){
	*ketemu = false;
	int i = 0;
	
	while(not *ketemu and i < counter_anggota) {
		if(agt[i].id_anggota == cari_id)
			*ketemu = true;
		else 		
			i++;
	}
	
	*posisi_ketemu = i;
}

/*
	Procedure untuk mencari anggota berdasar nama
*/
void cari_nama_anggota(char cari_nama[], bool *ketemu, int *posisi_ketemu){
	*ketemu = false;
	int i = 0;
	
	while(not *ketemu and i < counter_anggota) {
		if(strcmp(agt[i].nama_anggota, cari_nama) == 0)
			*ketemu = true;
		else 
			i++;
	}
	
	*posisi_ketemu = i;
}

/*
	Procedure untuk mencari ID simpanan wajib anggota
*/
void cari_id_simpanan_wajib(int cari_id, int indeks_anggota, bool *ketemu, int *posisi_ketemu){
	*ketemu = false;
	int i = 0;
	
	while(not *ketemu and i < indeks_simpanan_wajib[indeks_anggota]) {
		if(agt[indeks_anggota].simpanan_wajib[i].id_simpanan == cari_id)
			*ketemu = true;
		else 		
			i++;
	}
	
	*posisi_ketemu = i;
}

/*
	Procedure untuk mencari ID simpanan sukarela anggota
*/
void cari_id_simpanan_sukarela(int cari_id, int indeks_anggota, bool *ketemu, int *posisi_ketemu){
	*ketemu = false;
	int i = 0;
	
	while(not *ketemu and i < indeks_simpanan_sukarela[indeks_anggota]) {
		if(agt[indeks_anggota].simpanan_sukarela[i].id_simpanan == cari_id)
			*ketemu = true;
		else 		
			i++;
	}
	
	*posisi_ketemu = i;
}

/*
	Procedure untuk mencari ID ambil simpanan sukarela anggota
*/
void cari_id_ambil_sukarela(int cari_id, int indeks_anggota, bool *ketemu, int *posisi_ketemu){
	*ketemu = false;
	int i = 0;
	
	while(not *ketemu and i < indeks_ambil_sukarela[indeks_anggota]) {
		if(agt[indeks_anggota].ambil_simpanan_sukarela[i].id_ambil == cari_id)
			*ketemu = true;
		else 		
			i++;
	}
	
	*posisi_ketemu = i;
}


/*
	Procedure untuk mencari pinjaman berdasar id
*/
void cari_id_pinjam(int cari_id, int indeks_anggota, bool *ketemu, int *posisi_ketemu){
	*ketemu = false;
	int i = 0;
	
	while(not *ketemu and i < indeks_pinjaman[indeks_anggota]) {
		if(agt[indeks_anggota].pinjam[i].id_pinjam == cari_id)
			*ketemu = true;
		else 		
			i++;
	}
	
	*posisi_ketemu = i;
}

/*
	Function untuk mencari status pinjaman
*/
bool cari_status_pinjam(int indeks_anggota){
	bool ketemu = false;
	int i = 0;
	
	while(not ketemu and i < indeks_pinjaman[indeks_anggota]) {
		if(agt[indeks_anggota].pinjam[i].status_pinjam == 1)
			ketemu = true;
		else 		
			i++;
	}
	
	return ketemu;
}

/*
	Procedure untuk registrasi anggota baru
*/
void registrasi_anggota(){	
	int temp_id; //Untuk validasi ID duplikat atau tidak
	bool ketemu;
	int posisi_ketemu;
	
	//Tanggal sekarang
	int tgl, bln, thn;
	date_now(&tgl, &bln, &thn);
	
	system("CLS");				
	cout<<"---------------------------------------------------"<<endl;
	cout<<"               FORM REGISTRASI ANGGOTA             "<<endl;
	cout<<"---------------------------------------------------"<<endl;
	set_text_color(3);
    cout<<"Tanggal Registrasi \t\t\t "<<tgl<<"-"<<bln<<"-"<<thn<<endl;
    agt[counter_anggota].tgl_masuk_anggota.tgl = tgl;
    agt[counter_anggota].tgl_masuk_anggota.bln = bln;
    agt[counter_anggota].tgl_masuk_anggota.thn = thn;
    set_text_color(7);
	cout<<"---------------------------------------------------"<<endl;
    
    cout<<"ID Anggota \t\t\t\t: ";
    cin>>temp_id;
    
    //Validasi
   	cari_id_anggota(temp_id, &ketemu, &posisi_ketemu);   	
	while(ketemu == 1){
	   	set_text_color(12);
	   	cout<<"ID Tidak Boleh Duplikat!";
	   	set_text_color(7);
	   	getch();
	   	
	    cout<<endl<<"ID Anggota \t\t\t\t: ";
	    cin>>temp_id;
	    
	   	cari_id_anggota(temp_id, &ketemu, &posisi_ketemu);
	}  
	//End Validasi
    
    agt[counter_anggota].id_anggota = temp_id;
    
    cout<<"Nama Anggota \t\t\t\t: ";
    fflush(stdin);
    cin.get(agt[counter_anggota].nama_anggota, 25);
     	
    cout<<endl;
    cout<<"ALAMAT"<<endl;
	cout<<"---------------------------------------------------"<<endl;	
    cout<<"Nama Jalan/Komplek \t\t\t: ";
    fflush(stdin);
    cin.get(agt[counter_anggota].alamat_anggota.jalan_komplek, 50);	
    
    cout<<"Nama Kelurahan \t\t\t\t: ";
    fflush(stdin);
    cin.get(agt[counter_anggota].alamat_anggota.kelurahan, 30);
		
    cout<<"Nama Kecamatan \t\t\t\t: ";
    fflush(stdin);
    cin.get(agt[counter_anggota].alamat_anggota.kecamatan, 30);	
    
    cout<<"Nama Kota \t\t\t\t: ";
    fflush(stdin);
    cin.get(agt[counter_anggota].alamat_anggota.kota, 30);
		
    cout<<"Kode Pos \t\t\t\t: ";
    fflush(stdin);
    cin.get(agt[counter_anggota].alamat_anggota.kode_pos, 6);
	cout<<"---------------------------------------------------"<<endl<<endl;
    
    cout<<"Jenis Kelamin [L/P] \t\t\t: ";
    fflush(stdin);
    agt[counter_anggota].jk_anggota = toupper(getche());
    
    cout<<endl;  	
    cout<<"Tanggal Lahir [DD(spasi)MM(spasi)YYYY] \t: ";
    fflush(stdin);
    cin>>agt[counter_anggota].tgl_lahir_anggota.tgl>>agt[counter_anggota].tgl_lahir_anggota.bln>>agt[counter_anggota].tgl_lahir_anggota.thn;
    
    cout<<"Nomer Telepon \t\t\t\t: ";
    fflush(stdin);
    cin.get(agt[counter_anggota].notelp_anggota, 17);
     	
    cout<<"Simpanan Pokok \t\t\t\t: Rp. 200.000,-"<<endl;
    agt[counter_anggota].simpanan_pokok = SIMPANAN_POKOK;
    
	cout<<"---------------------------------------------------"<<endl;
	
	//Init total simpanan sukarela
	agt[counter_anggota].total_simpanan_sukarela = 0;    
		     
	counter_anggota += 1;  	
	
	keuangan_koperasi();
	
	cout<<endl<<endl;
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk list anggota yang terdaftar
*/
void list_anggota(){
	system("CLS");				
	cout<<"---------------------------------------------------"<<endl;
	cout<<"                    DATA ANGGOTA                   "<<endl;
	cout<<"---------------------------------------------------"<<endl;	
	set_text_color(3);
	cout<<"Jumlah Anggota \t\t: "<<counter_anggota<<endl;
    set_text_color(7);
	cout<<"---------------------------------------------------"<<endl<<endl;
	
	for (int i = 0; i < counter_anggota; i++){
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(2);
    	cout<<"DATA KE - "<<i + 1<<endl;
		set_text_color(7);
		cout<<"---------------------------------------------------"<<endl;		
     	cout<<"ID Anggota \t\t: "<<agt[i].id_anggota<<endl;
     	
     	cout<<"Nama Anggota \t\t: "<<agt[i].nama_anggota<<endl;
     	
		cout<<"Tanggal Masuk \t\t: "<<agt[i].tgl_masuk_anggota.tgl<<"-"<<agt[i].tgl_masuk_anggota.bln<<"-"<<agt[i].tgl_masuk_anggota.thn<<endl;
		
     	cout<<endl;
     	cout<<"ALAMAT"<<endl;
		cout<<"---------------------------------------------------"<<endl; 	
     	cout<<"Nama Jalan/Komplek \t: "<<agt[i].alamat_anggota.jalan_komplek<<endl;	
     	
     	cout<<"Nama Kelurahan \t\t: "<<agt[i].alamat_anggota.kelurahan<<endl;
		 	
     	cout<<"Nama Kecamatan \t\t: "<<agt[i].alamat_anggota.kecamatan<<endl;	
     	
     	cout<<"Nama Kota \t\t: "<<agt[i].alamat_anggota.kota<<endl;
		 	
     	cout<<"Kode Pos \t\t: "<<agt[i].alamat_anggota.kode_pos<<endl;
		cout<<"---------------------------------------------------"<<endl<<endl;
     	
     	cout<<"Jenis Kelamin \t\t: "<<agt[i].jk_anggota<<endl;
     		
     	cout<<"Tanggal Lahir \t\t: ";
     	cout<<agt[i].tgl_lahir_anggota.tgl<<"-"<<agt[i].tgl_lahir_anggota.bln<<"-"<<agt[i].tgl_lahir_anggota.thn<<endl;
     	
    	cout<<"Nomer Telepon \t\t: "<<agt[i].notelp_anggota<<endl;
     	
     	cout<<"Saldo S. Sukarela \t: "<<agt[i].total_simpanan_sukarela<<endl;
		cout<<"---------------------------------------------------"<<endl;
		cout<<endl<<endl;  
	}
     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk list anggota yang terdaftar berdasar pencarian menurut ID
*/
void list_cari_id_anggota(){
	int cari_id;
	bool ketemu = false;
	int posisi_ketemu;
	
	system("CLS");				
	cout<<"---------------------------------------------------"<<endl;
	cout<<"                    DATA ANGGOTA                   "<<endl;
	cout<<"---------------------------------------------------"<<endl;
	cout<<"Masukkan ID Anggota \t: ";
	cin>>cari_id;
	cout<<"---------------------------------------------------"<<endl;
	cari_id_anggota(cari_id, &ketemu, &posisi_ketemu);
	
	if(ketemu){	
		cout<<endl;
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(2);
    	cout<<"DATA YANG DICARI"<<endl;
		set_text_color(7);
		cout<<"---------------------------------------------------"<<endl;	
		
     	cout<<"ID Anggota \t\t: "<<agt[posisi_ketemu].id_anggota<<endl;
     	
     	cout<<"Nama Anggota \t\t: "<<agt[posisi_ketemu].nama_anggota<<endl;
     	
		cout<<"Tanggal Masuk \t\t: "<<agt[posisi_ketemu].tgl_masuk_anggota.tgl<<"-"<<agt[posisi_ketemu].tgl_masuk_anggota.bln<<"-"<<agt[posisi_ketemu].tgl_masuk_anggota.thn<<endl;
		
     	cout<<"ALAMAT"<<endl;
		cout<<"---------------------------------------------------"<<endl; 	
     	cout<<"Nama Jalan/Komplek \t: "<<agt[posisi_ketemu].alamat_anggota.jalan_komplek<<endl;	
     	
     	cout<<"Nama Kelurahan \t\t: "<<agt[posisi_ketemu].alamat_anggota.kelurahan<<endl;
		 	
     	cout<<"Nama Kecamatan \t\t: "<<agt[posisi_ketemu].alamat_anggota.kecamatan<<endl;	
     	
     	cout<<"Nama Kota \t\t: "<<agt[posisi_ketemu].alamat_anggota.kota<<endl;
		 	
     	cout<<"Kode Pos \t\t: "<<agt[posisi_ketemu].alamat_anggota.kode_pos<<endl;
		cout<<"---------------------------------------------------"<<endl<<endl;
     	
     	cout<<"Jenis Kelamin \t\t: "<<agt[posisi_ketemu].jk_anggota<<endl;
     		
     	cout<<"Tanggal Lahir \t\t: ";
     	cout<<agt[posisi_ketemu].tgl_lahir_anggota.tgl<<"-"<<agt[posisi_ketemu].tgl_lahir_anggota.bln<<"-"<<agt[posisi_ketemu].tgl_lahir_anggota.thn<<endl;
     	
    	cout<<"Nomer Telepon \t\t: "<<agt[posisi_ketemu].notelp_anggota<<endl;
     	
		cout<<"---------------------------------------------------"<<endl;
		cout<<endl<<endl;  
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk list anggota yang terdaftar berdasar pencarian menurut nama
*/
void list_cari_nama_anggota(){
	char cari_nama[26];
	bool ketemu = false;
	int posisi_ketemu;
	
	system("CLS");				
	cout<<"---------------------------------------------------"<<endl;
	cout<<"                    DATA ANGGOTA                   "<<endl;
	cout<<"---------------------------------------------------"<<endl;
	cout<<"Masukkan Nama Anggota \t: ";
	fflush(stdin);
	cin.get(cari_nama, 25);
	cout<<"---------------------------------------------------"<<endl;
	cari_nama_anggota(cari_nama, &ketemu, &posisi_ketemu);
	
	if(ketemu){	
		cout<<endl;
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(2);
    	cout<<"DATA YANG DICARI"<<endl;
		set_text_color(7);
		cout<<"---------------------------------------------------"<<endl;	
		
     	cout<<"ID Anggota \t\t: "<<agt[posisi_ketemu].id_anggota<<endl;
     	
     	cout<<"Nama Anggota \t\t: "<<agt[posisi_ketemu].nama_anggota<<endl;
     	
		cout<<"Tanggal Masuk \t\t: "<<agt[posisi_ketemu].tgl_masuk_anggota.tgl<<"-"<<agt[posisi_ketemu].tgl_masuk_anggota.bln<<"-"<<agt[posisi_ketemu].tgl_masuk_anggota.thn<<endl;
		
     	cout<<endl;
     	cout<<"ALAMAT"<<endl;
		cout<<"---------------------------------------------------"<<endl; 	
     	cout<<"Nama Jalan/Komplek \t: "<<agt[posisi_ketemu].alamat_anggota.jalan_komplek<<endl;	
     	
     	cout<<"Nama Kelurahan \t\t: "<<agt[posisi_ketemu].alamat_anggota.kelurahan<<endl;
		 	
     	cout<<"Nama Kecamatan \t\t: "<<agt[posisi_ketemu].alamat_anggota.kecamatan<<endl;	
     	
     	cout<<"Nama Kota \t\t: "<<agt[posisi_ketemu].alamat_anggota.kota<<endl;
		 	
     	cout<<"Kode Pos \t\t: "<<agt[posisi_ketemu].alamat_anggota.kode_pos<<endl;
		cout<<"---------------------------------------------------"<<endl<<endl;
     	
     	cout<<"Jenis Kelamin \t\t: "<<agt[posisi_ketemu].jk_anggota<<endl;
     		
     	cout<<"Tanggal Lahir \t\t: ";
     	cout<<agt[posisi_ketemu].tgl_lahir_anggota.tgl<<"-"<<agt[posisi_ketemu].tgl_lahir_anggota.bln<<"-"<<agt[posisi_ketemu].tgl_lahir_anggota.thn<<endl;
     	
    	cout<<"Nomer Telepon \t\t: "<<agt[posisi_ketemu].notelp_anggota<<endl;
     	
		cout<<"---------------------------------------------------"<<endl;
		cout<<endl<<endl;  
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk edit anggota
*/
void edit_anggota(){
	int cari_id;
	bool ketemu = false;
	int posisi_ketemu;
	
	system("CLS");				
	cout<<"---------------------------------------------------"<<endl;
	cout<<"                    DATA ANGGOTA                   "<<endl;
	cout<<"---------------------------------------------------"<<endl;
	cout<<"Masukkan ID Anggota \t\t\t: ";
	cin>>cari_id;
	cout<<"---------------------------------------------------"<<endl;
	cari_id_anggota(cari_id, &ketemu, &posisi_ketemu);
	
	if(ketemu){	
     	cout<<"Nama Anggota \t\t\t\t: ";
	    fflush(stdin);
	    cin.get(agt[posisi_ketemu].nama_anggota, 25);
	     	
	    cout<<endl;
	    cout<<"ALAMAT"<<endl;
		cout<<"---------------------------------------------------"<<endl;	
	    cout<<"Nama Jalan/Komplek \t\t\t: ";
	    fflush(stdin);
	    cin.get(agt[posisi_ketemu].alamat_anggota.jalan_komplek, 50);	
	    
	    cout<<"Nama Kelurahan \t\t\t\t: ";
	    fflush(stdin);
	    cin.get(agt[posisi_ketemu].alamat_anggota.kelurahan, 30);
			
	    cout<<"Nama Kecamatan \t\t\t\t: ";
	    fflush(stdin);
	    cin.get(agt[posisi_ketemu].alamat_anggota.kecamatan, 30);	
	    
	    cout<<"Nama Kota \t\t\t\t: ";
	    fflush(stdin);
	    cin.get(agt[posisi_ketemu].alamat_anggota.kota, 30);
			
	    cout<<"Kode Pos \t\t\t\t: ";
	    fflush(stdin);
	    cin.get(agt[posisi_ketemu].alamat_anggota.kode_pos, 6);
		cout<<"---------------------------------------------------"<<endl<<endl;
	    
	    cout<<"Jenis Kelamin [L/P] \t\t\t: ";
	    fflush(stdin);
	    agt[posisi_ketemu].jk_anggota = getche();
	    
	    cout<<endl;  	
	    cout<<"Tanggal Lahir [DD(spasi)MM(spasi)YYYY] \t: ";
	    fflush(stdin);
	    cin>>agt[posisi_ketemu].tgl_lahir_anggota.tgl>>agt[posisi_ketemu].tgl_lahir_anggota.bln>>agt[posisi_ketemu].tgl_lahir_anggota.thn;
	    
	    cout<<"Nomer Telepon \t\t\t\t: ";
	    fflush(stdin);
	    cin.get(agt[posisi_ketemu].notelp_anggota, 17);\
	    
		cout<<"---------------------------------------------------"<<endl;
		cout<<endl<<endl;
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk hapus anggota
*/
void hapus_anggota(){
	int temp_id;
	bool ketemu = false;
	int posisi_ketemu;
	
	system("CLS");				
	cout<<"---------------------------------------------------"<<endl;
	cout<<"                    DATA ANGGOTA                   "<<endl;
	cout<<"---------------------------------------------------"<<endl;
	cout<<"Masukkan ID Anggota \t: ";
	cin>>temp_id;
	cout<<"---------------------------------------------------"<<endl;
	cari_id_anggota(temp_id, &ketemu, &posisi_ketemu);
	
	if(ketemu){	
		set_text_color(10);
		cout<<"Anggota Dengan ID "<<agt[posisi_ketemu].id_anggota<<" Berhasil Dihapus!"<<endl;
		set_text_color(7);
     	for(int i = posisi_ketemu + 1; i < counter_anggota; i++)
     		agt[i - 1] = agt[i];
			 
		counter_anggota -= 1;	 
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}     
     
	keuangan_koperasi();
     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk menu anggota
*/
void menu_anggota(){
	char menu;
	do {
		system("CLS");		
		cout<<"-------------------------------------"<<endl;
		cout<<"             MENU ANGGOTA            "<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"1. Registrasi Anggota"<<endl;
		cout<<"2. Daftar Anggota"<<endl;
		cout<<"3. Cari Anggota Berdasar ID"<<endl;
		cout<<"4. Cari Anggota Berdasar Nama"<<endl;
		cout<<"5. Edit Anggota"<<endl;
		cout<<"6. Hapus Anggota"<<endl;
		cout<<"7. Kembali"<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"Masukkan Pilihan : ";
		cin>>menu;
		
		switch(menu){
			case '1' : 
				registrasi_anggota();
				break;
					 
			case '2' : 
				list_anggota();
				break;
			
			case '3' :
				list_cari_id_anggota();
				break;
				
			case '4' :
				list_cari_nama_anggota();
				break;
													
			case '5' :
				edit_anggota();
				break;
				
			case '6' :
				hapus_anggota();
				break;	
				
			case '7' :
				break;
			
			default :
				set_text_color(12);
				cout<<"MENU TIDAK ADA!";
				set_text_color(7);
				fflush(stdin);
				getch();
		}
	} while(menu != '7');
}

/*
	Procedure untuk simpanan wajib
*/
void simpanan_wajib(){	
	int cari_id;
	int temp_id;
	int i = 0; //Counter untuk pencarian id anggota
	int i2 = 0; //Counter untuk pencarian bulan thn
	bool ketemu = false;
	bool ketemu_bulan = false;
	bool ketemu_id;
	int posisi_ketemu;

	//Tanggal sekarang
	int tgl, bln, thn;
	
	system("CLS");
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"               SIMPANAN WAJIB BULANAN              "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"Masukkan ID anggota \t: ";
	cin>>cari_id;
	cout<<"---------------------------------------------------"<<endl; 
	
	//Pencarian ID Anggota
	cari_id_anggota(cari_id, &ketemu, &i);
	
	if(ketemu){
		cout<<"Tanggal Simpanan (Untuk Bulan Berapa) [DD(spasi)MM(spasi)YYYY] \t: ";
    	fflush(stdin);
    	cin>>tgl>>bln>>thn;
		while(not ketemu_bulan and i2 < indeks_simpanan_wajib[i]) {
			if(agt[i].simpanan_wajib[i2].tgl_simpanan.bln == bln && agt[i].simpanan_wajib[i2].tgl_simpanan.thn == thn)
				ketemu_bulan = true;
			else 
				i2++;
		}
		
		if(ketemu_bulan){
			set_text_color(12);
			cout<<"Simpanan Wajib Bulan Ini Sudah Dibayar!"<<endl;
			set_text_color(7);
		}else{
			set_text_color(3);
		    cout<<"Tanggal Simpanan \t: "<<tgl<<"-"<<bln<<"-"<<thn<<endl;
			set_text_color(7);
			cout<<"---------------------------------------------------"<<endl; 
			agt[i].simpanan_wajib[indeks_simpanan_wajib[i]].tgl_simpanan.tgl = tgl;
			agt[i].simpanan_wajib[indeks_simpanan_wajib[i]].tgl_simpanan.bln = bln;
			agt[i].simpanan_wajib[indeks_simpanan_wajib[i]].tgl_simpanan.thn = thn;
		    
		    cout<<"ID Simpanan \t\t: ";
		    cin>>temp_id;
		    
		    //Validasi
		    for(int j = 0; j < counter_anggota; j++){
			   	cari_id_simpanan_wajib(temp_id, j, &ketemu_id, &posisi_ketemu);  
				   
				if(ketemu_id == 1)
					break; 	 
			}		
			
			while(ketemu_id == 1){
			   	set_text_color(12);
			   	cout<<"ID Tidak Boleh Duplikat!";
			   	set_text_color(7);
			   	getch();
			   	
			    cout<<endl<<"ID Simpanan \t\t: ";
			    cin>>temp_id;
			    
				for(int j = 0; j < counter_anggota; j++){
			   		cari_id_simpanan_wajib(temp_id, j, &ketemu_id, &posisi_ketemu);   
						   
					if(ketemu_id == 1)
						break; 	 	 
				} 
			} 
			//End Validasi
	
			agt[i].simpanan_wajib[indeks_simpanan_wajib[i]].id_simpanan = temp_id;
	
			cout<<"Jumlah Simpanan \t: Rp. 50000,-";
			agt[i].simpanan_wajib[indeks_simpanan_wajib[i]].jml_simpanan = SIMPANAN_WAJIB;
			
			indeks_simpanan_wajib[i] += 1;
			
			keuangan_koperasi();
			
			cout<<endl<<endl;
		}
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}

	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk melihat seluruh transaksi simpanan wajib
*/
void lihat_simpanan_wajib(){
	system("CLS");		
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"                 LIHAT SIMPANAN WAJIB              "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
		
	cout<<endl;
	for(int i = 0; i < counter_anggota; i++){		
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(2);
	    cout<<"DATA ANGGOTA - "<<agt[i].id_anggota<<" - "<<agt[i].nama_anggota<<endl;
		set_text_color(7);
		
		for(int j = 0; j < indeks_simpanan_wajib[i]; j++){	
			cout<<"---------------------------------------------------"<<endl;
			set_text_color(3);
		    cout<<"DATA KE - "<<j + 1<<endl;
			set_text_color(7);
			cout<<"---------------------------------------------------"<<endl;	
	     	cout<<"ID Simpanan \t\t: "<<agt[i].simpanan_wajib[j].id_simpanan<<endl;
	     	cout<<"Tanggal Simpanan \t: "<<agt[i].simpanan_wajib[j].tgl_simpanan.tgl<<"-"<<agt[i].simpanan_wajib[j].tgl_simpanan.bln<<"-"<<agt[i].simpanan_wajib[j].tgl_simpanan.thn<<endl;
	     	cout<<"Jumlah Simpanan \t: Rp. "<<agt[i].simpanan_wajib[j].jml_simpanan<<endl;	 	     	
			cout<<"---------------------------------------------------"<<endl;
		}
		cout<<endl<<endl;
	}
     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk melihat seluruh transaksi simpanan wajib berdasarkan ID anggota
*/
void lihat_simpanan_wajib_anggota(){
	int temp_id;
	bool ketemu = false;
	int posisi_ketemu;
		
	system("CLS");		
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"                 LIHAT SIMPANAN WAJIB              "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"Masukkan ID Anggota \t: ";
	cin>>temp_id;
	cout<<"---------------------------------------------------"<<endl; 
				
	cari_id_anggota(temp_id, &ketemu, &posisi_ketemu);
		
	if(ketemu){
		cout<<endl;
		for(int i = 0; i < indeks_simpanan_wajib[posisi_ketemu]; i++){	
			cout<<"---------------------------------------------------"<<endl;
			set_text_color(2);
		    cout<<"DATA KE - "<<i + 1<<endl;
			set_text_color(7);
			cout<<"---------------------------------------------------"<<endl;	
	     	cout<<"ID Simpanan \t\t: "<<agt[posisi_ketemu].simpanan_wajib[i].id_simpanan<<endl;
	     	cout<<"Tanggal Simpanan \t: "<<agt[posisi_ketemu].simpanan_wajib[i].tgl_simpanan.tgl<<"-"<<agt[posisi_ketemu].simpanan_wajib[i].tgl_simpanan.bln<<"-"<<agt[posisi_ketemu].simpanan_wajib[i].tgl_simpanan.thn<<endl;
	     	cout<<"Jumlah Simpanan \t: Rp. "<<agt[posisi_ketemu].simpanan_wajib[i].jml_simpanan<<endl;	     	
			cout<<"---------------------------------------------------"<<endl;
			cout<<endl<<endl;
		}
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
	     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk melihat transaksi simpanan wajib berdasarkan ID Simpanan
*/
void lihat_simpanan_wajib_idsimpanan(){
	int temp_id;
	bool ketemu_id = false;
	int posisi_ketemu;
	int i = 0;	
		
	system("CLS");		
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"                 LIHAT SIMPANAN WAJIB              "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"Masukkan ID Simpanan \t: ";
	cin>>temp_id;
	cout<<"---------------------------------------------------"<<endl; 
				
	for(i = 0; i < counter_anggota; i++){
	   	cari_id_simpanan_wajib(temp_id, i, &ketemu_id, &posisi_ketemu);  
		   
		if(ketemu_id == 1)
			break; 	 
	}
	
	if(ketemu_id){
		cout<<endl;
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(2);
		cout<<"DATA YANG DICARI"<<endl;
		set_text_color(7);
		cout<<"---------------------------------------------------"<<endl;	
	    cout<<"ID Simpanan \t\t: "<<agt[i].simpanan_wajib[posisi_ketemu].id_simpanan<<endl;
	    cout<<"Tanggal Simpanan \t: "<<agt[i].simpanan_wajib[posisi_ketemu].tgl_simpanan.tgl<<"-"<<agt[i].simpanan_wajib[posisi_ketemu].tgl_simpanan.bln<<"-"<<agt[i].simpanan_wajib[posisi_ketemu].tgl_simpanan.thn<<endl;
	    cout<<"Jumlah Simpanan \t: Rp. "<<agt[i].simpanan_wajib[posisi_ketemu].jml_simpanan<<endl;	     	
		cout<<"---------------------------------------------------"<<endl;
		cout<<endl<<endl;
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
	     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk memilih jenis simpanan
*/
void menu_simpanan_wajib(){
	char menu;
	do {
		system("CLS");		
		cout<<"-------------------------------------"<<endl;
		cout<<"             MENU SIMPANAN           "<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"1. Simpan"<<endl;
		cout<<"2. Lihat Semua Transaksi"<<endl;
		cout<<"3. Lihat Semua Transaksi Berdasarkan ID Anggota"<<endl;
		cout<<"4. Cari Berdasarkan ID Simpanan"<<endl;
		cout<<"5. Kembali"<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"Masukkan Pilihan : ";
		cin>>menu;
		
		switch(menu){
			case '1' : 
				simpanan_wajib();
				break;
					 
			case '2' : 
				lihat_simpanan_wajib();
				break;
				
			case '3' :
				lihat_simpanan_wajib_anggota();
				break;
			
			case '4' :
				lihat_simpanan_wajib_idsimpanan();
				break;
				
			case '5' :
				break;
			
			default :
				set_text_color(12);
				cout<<"MENU TIDAK ADA!";
				set_text_color(7);
				fflush(stdin);
				getch();
		}
	} while(menu != '5');
}

/*
	Procedure untuk simpanan sukarela
*/
void simpanan_sukarela(){	
	int cari_id;
	int temp_id;
	int i = 0;
	bool ketemu = false;
	int posisi_ketemu;
	bool ketemu_id = false;

	//Tanggal sekarang
	int tgl, bln, thn;
	date_now(&tgl, &bln, &thn);
	
	system("CLS");
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"                  SIMPANAN SUKARELA                "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"Masukkan ID anggota : ";
	cin>>cari_id;
	cout<<"---------------------------------------------------"<<endl; 
	
	//Pencarian ID Anggota
	cari_id_anggota(cari_id, &ketemu, &i);
	
	if(ketemu) {
		set_text_color(3);
	    cout<<"Tanggal Simpanan \t: "<<tgl<<"-"<<bln<<"-"<<thn<<endl;
	    set_text_color(7);
		cout<<"---------------------------------------------------"<<endl; 
		agt[i].simpanan_sukarela[indeks_simpanan_sukarela[i]].tgl_simpanan.tgl = tgl;
		agt[i].simpanan_sukarela[indeks_simpanan_sukarela[i]].tgl_simpanan.bln = bln;
		agt[i].simpanan_sukarela[indeks_simpanan_sukarela[i]].tgl_simpanan.thn = thn;
	    
	    cout<<"ID Simpanan \t\t: ";
		cin>>temp_id;
		    
		//Validasi
		for(int j = 0; j < counter_anggota; j++){
			cari_id_simpanan_sukarela(temp_id, j, &ketemu_id, &posisi_ketemu);   
					   
			if(ketemu_id == 1)
				break; 	 	 
		}		
			
		while(ketemu_id == 1){
		   	set_text_color(12);
		   	cout<<"ID Tidak Boleh Duplikat!";
		   	set_text_color(7);
		   	getch();
			   	
		    cout<<endl<<"ID Simpanan \t\t: ";
		    cin>>temp_id;
		    
			for(int j = 0; j < counter_anggota; j++){
		   		cari_id_simpanan_sukarela(temp_id, j, &ketemu_id, &posisi_ketemu);   
					   
				if(ketemu_id == 1)
					break; 	 	 
			} 
		} 
		//End Validasi
	
		agt[i].simpanan_sukarela[indeks_simpanan_sukarela[i]].id_simpanan = temp_id;

		cout<<"Jumlah Simpanan \t: ";
		cin>>agt[i].simpanan_sukarela[indeks_simpanan_sukarela[i]].jml_simpanan;
		
		agt[i].total_simpanan_sukarela += agt[i].simpanan_sukarela[indeks_simpanan_sukarela[i]].jml_simpanan;
		
		indeks_simpanan_sukarela[i] += 1;
		
		keuangan_koperasi();
		
		cout<<endl;	
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}

	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk melihat seluruh transaksi simpanan sukarela
*/
void lihat_simpanan_sukarela(){
	system("CLS");		
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"               LIHAT SIMPANAN SUKARELA             "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
		
	cout<<endl;
	for(int i = 0; i < counter_anggota; i++){		
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(2);
	    cout<<"DATA ANGGOTA - "<<agt[i].id_anggota<<" - "<<agt[i].nama_anggota<<endl;
		set_text_color(7);
		
		for(int j = 0; j < indeks_simpanan_sukarela[i]; j++){	
			cout<<"---------------------------------------------------"<<endl;
			set_text_color(3);
		    cout<<"DATA KE - "<<j + 1<<endl;
			set_text_color(7);
			cout<<"---------------------------------------------------"<<endl;	
	     	cout<<"ID Simpanan \t\t: "<<agt[i].simpanan_sukarela[j].id_simpanan<<endl;
	     	cout<<"Tanggal Simpanan \t: "<<agt[i].simpanan_sukarela[j].tgl_simpanan.tgl<<"-"<<agt[i].simpanan_sukarela[j].tgl_simpanan.bln<<"-"<<agt[i].simpanan_sukarela[j].tgl_simpanan.thn<<endl;
	     	cout<<"Jumlah Simpanan \t: Rp. "<<agt[i].simpanan_sukarela[j].jml_simpanan<<endl;	 	     	
			cout<<"---------------------------------------------------"<<endl;
		}
		cout<<endl<<endl;
	}
     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk melihat seluruh transaksi simpanan sukarela berdasarkan ID anggota
*/
void lihat_simpanan_sukarela_anggota(){
	int temp_id;
	bool ketemu = false;
	int posisi_ketemu;
		
	system("CLS");		
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"               LIHAT SIMPANAN SUKARELA             "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"Masukkan ID Anggota \t: ";
	cin>>temp_id;
	cout<<"---------------------------------------------------"<<endl; 
				
	cari_id_anggota(temp_id, &ketemu, &posisi_ketemu);
		
	if(ketemu){
		cout<<endl;
		for(int i = 0; i < indeks_simpanan_sukarela[posisi_ketemu]; i++){	
			cout<<"---------------------------------------------------"<<endl;
			set_text_color(2);
		    cout<<"DATA KE - "<<i + 1<<endl;
			set_text_color(7);
			cout<<"---------------------------------------------------"<<endl;	
	     	cout<<"ID Simpanan \t\t: "<<agt[posisi_ketemu].simpanan_sukarela[i].id_simpanan<<endl;
	     	cout<<"Tanggal Simpanan \t: "<<agt[posisi_ketemu].simpanan_sukarela[i].tgl_simpanan.tgl<<"-"<<agt[posisi_ketemu].simpanan_sukarela[i].tgl_simpanan.bln<<"-"<<agt[posisi_ketemu].simpanan_sukarela[i].tgl_simpanan.thn<<endl;
	     	cout<<"Jumlah Simpanan \t: Rp. "<<agt[posisi_ketemu].simpanan_sukarela[i].jml_simpanan<<endl;	     	
			cout<<"---------------------------------------------------"<<endl;
			cout<<endl<<endl;
		}
		
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(3);
		cout<<"Saldo \t\t\t: "<<agt[posisi_ketemu].total_simpanan_sukarela<<endl;
		set_text_color(7);
		cout<<"---------------------------------------------------"<<endl;
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
	     
	cout<<"Press Any Key to Continue...";
	getch();
}


/*
	Procedure untuk melihat transaksi simpanan sukarela berdasarkan ID Simpanan
*/
void lihat_simpanan_sukarela_idsimpanan(){
	int temp_id;
	bool ketemu_id = false;
	int posisi_ketemu;
	int i = 0;	
		
	system("CLS");		
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"               LIHAT SIMPANAN SUKARELA             "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"Masukkan ID Simpanan \t: ";
	cin>>temp_id;
	cout<<"---------------------------------------------------"<<endl; 
				
	for(i = 0; i < counter_anggota; i++){
	   	cari_id_simpanan_sukarela(temp_id, i, &ketemu_id, &posisi_ketemu);  
		   
		if(ketemu_id == 1)
			break; 	 
	}
	
	if(ketemu_id){
		cout<<endl;
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(2);
		cout<<"DATA YANG DICARI"<<endl;
		set_text_color(7);
		cout<<"---------------------------------------------------"<<endl;	
	    cout<<"ID Simpanan \t\t: "<<agt[i].simpanan_sukarela[posisi_ketemu].id_simpanan<<endl;
	    cout<<"Tanggal Simpanan \t: "<<agt[i].simpanan_sukarela[posisi_ketemu].tgl_simpanan.tgl<<"-"<<agt[i].simpanan_sukarela[posisi_ketemu].tgl_simpanan.bln<<"-"<<agt[i].simpanan_sukarela[posisi_ketemu].tgl_simpanan.thn<<endl;
	    cout<<"Jumlah Simpanan \t: Rp. "<<agt[i].simpanan_sukarela[posisi_ketemu].jml_simpanan<<endl;	     	
		cout<<"---------------------------------------------------"<<endl;
		cout<<endl<<endl;
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
	     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure ambil simpanan sukarela
*/
void ambil_simpanan_sukarela(){	
	int cari_id;
	int temp_id;
	int i = 0;
	bool ketemu = false;
	int posisi_ketemu;
	bool ketemu_id = false;

	//Tanggal sekarang
	int tgl, bln, thn;
	date_now(&tgl, &bln, &thn);
	
	system("CLS");
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"               AMBIL SIMPANAN SUKARELA             "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"Masukkan ID anggota : ";
	cin>>cari_id;
	cout<<"---------------------------------------------------"<<endl; 
	
	//Pencarian ID Anggota
	cari_id_anggota(cari_id, &ketemu, &i);
	
	if(ketemu) {
		if(agt[i].total_simpanan_sukarela > 0){
			set_text_color(3);
		    cout<<"Tanggal Ambil \t\t: "<<tgl<<"-"<<bln<<"-"<<thn<<endl;
		    set_text_color(7);
			cout<<"---------------------------------------------------"<<endl; 
			agt[i].ambil_simpanan_sukarela[indeks_ambil_sukarela[i]].tgl_ambil.tgl = tgl;
			agt[i].ambil_simpanan_sukarela[indeks_ambil_sukarela[i]].tgl_ambil.bln = bln;
			agt[i].ambil_simpanan_sukarela[indeks_ambil_sukarela[i]].tgl_ambil.thn = thn;
		    
		    cout<<"ID Ambil \t\t: ";
			cin>>temp_id;
			    
			//Validasi
			for(int j = 0; j < counter_anggota; j++){
				cari_id_ambil_sukarela(temp_id, j, &ketemu_id, &posisi_ketemu);   
						   
				if(ketemu_id == 1)
					break; 	 	 
			}		
				
			while(ketemu_id == 1){
			   	set_text_color(12);
			   	cout<<"ID Tidak Boleh Duplikat!";
			   	set_text_color(7);
			   	getch();
				   	
			    cout<<endl<<"ID Ambil \t\t: ";
			    cin>>temp_id;
			    
				for(int j = 0; j < counter_anggota; j++){
			   		cari_id_ambil_sukarela(temp_id, j, &ketemu_id, &posisi_ketemu);   
						   
					if(ketemu_id == 1)
						break; 	 	 
				} 
			} 
			//End Validasi
		
			agt[i].ambil_simpanan_sukarela[indeks_ambil_sukarela[i]].id_ambil = temp_id;
	
			jumlah:
			cout<<"Jumlah Pengambilan \t: ";
			cin>>agt[i].ambil_simpanan_sukarela[indeks_ambil_sukarela[i]].jml_ambil;
			if(agt[i].ambil_simpanan_sukarela[indeks_ambil_sukarela[i]].jml_ambil > agt[i].total_simpanan_sukarela){
				set_text_color(12);
				cout<<"Saldo Tidak Cukup!";
				set_text_color(7);
				getch();
				cout<<endl;
				goto jumlah;
			}
				
			agt[i].total_simpanan_sukarela -= agt[i].ambil_simpanan_sukarela[indeks_ambil_sukarela[i]].jml_ambil;
			
			indeks_ambil_sukarela[i] += 1;
			
			keuangan_koperasi();
			
			cout<<endl;	
		}else{
			set_text_color(12);
			cout<<"Maaf Saldo Anda 0!"<<endl;
			set_text_color(7);
		}
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}

	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk melihat seluruh transaksi pengambilan simpanan sukarela
*/
void lihat_ambil_sukarela(){
	system("CLS");		
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"         LIHAT PENGAMBILAN SIMPANAN SUKARELA       "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
		
	cout<<endl;
	for(int i = 0; i < counter_anggota; i++){		
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(2);
	    cout<<"DATA ANGGOTA - "<<agt[i].id_anggota<<" - "<<agt[i].nama_anggota<<endl;
		set_text_color(7);
		
		for(int j = 0; j < indeks_ambil_sukarela[i]; j++){	
			cout<<"---------------------------------------------------"<<endl;
			set_text_color(3);
		    cout<<"DATA KE - "<<j + 1<<endl;
			set_text_color(7);
			cout<<"---------------------------------------------------"<<endl;	
	     	cout<<"ID Ambil \t: "<<agt[i].ambil_simpanan_sukarela[j].id_ambil<<endl;
	     	cout<<"Tanggal Ambil \t: "<<agt[i].ambil_simpanan_sukarela[j].tgl_ambil.tgl<<"-"<<agt[i].ambil_simpanan_sukarela[j].tgl_ambil.bln<<"-"<<agt[i].ambil_simpanan_sukarela[j].tgl_ambil.thn<<endl;
	     	cout<<"Jumlah Ambil \t: Rp. "<<agt[i].ambil_simpanan_sukarela[j].jml_ambil<<endl;	 	     	
			cout<<"---------------------------------------------------"<<endl;
		}
		cout<<endl<<endl;
	}
     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk melihat seluruh transaksi pengambilan simpanan sukarela berdasarkan ID anggota
*/
void lihat_ambil_sukarela_anggota(){
	int temp_id;
	bool ketemu = false;
	int posisi_ketemu;
		
	system("CLS");		
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"         LIHAT PENGAMBILAN SIMPANAN SUKARELA       "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"Masukkan ID Anggota \t: ";
	cin>>temp_id;
	cout<<"---------------------------------------------------"<<endl; 
				
	cari_id_anggota(temp_id, &ketemu, &posisi_ketemu);
		
	if(ketemu){
		cout<<endl;
		for(int i = 0; i < indeks_ambil_sukarela[posisi_ketemu]; i++){	
			cout<<"---------------------------------------------------"<<endl;
			set_text_color(2);
		    cout<<"DATA KE - "<<i + 1<<endl;
			set_text_color(7);
			cout<<"---------------------------------------------------"<<endl;	
	     	cout<<"ID Ambil \t: "<<agt[posisi_ketemu].ambil_simpanan_sukarela[i].id_ambil<<endl;
	     	cout<<"Tanggal Ambil \t: "<<agt[posisi_ketemu].ambil_simpanan_sukarela[i].tgl_ambil.tgl<<"-"<<agt[posisi_ketemu].ambil_simpanan_sukarela[i].tgl_ambil.bln<<"-"<<agt[posisi_ketemu].ambil_simpanan_sukarela[i].tgl_ambil.thn<<endl;
	     	cout<<"Jumlah Ambil \t: Rp. "<<agt[posisi_ketemu].ambil_simpanan_sukarela[i].jml_ambil<<endl;	     	
			cout<<"---------------------------------------------------"<<endl;
			cout<<endl<<endl;
		}
		
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(3);
		cout<<"Saldo \t\t\t: "<<agt[posisi_ketemu].total_simpanan_sukarela<<endl;
		set_text_color(7);
		cout<<"---------------------------------------------------"<<endl;
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
	     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk melihat transaksi ambil simpanan sukarela berdasarkan ID Ambil
*/
void lihat_ambil_sukarela_idambil(){
	int temp_id;
	bool ketemu_id = false;
	int posisi_ketemu;
	int i = 0;	
		
	system("CLS");		
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"         LIHAT PENGAMBILAN SIMPANAN SUKARELA       "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"Masukkan ID Ambil \t: ";
	cin>>temp_id;
	cout<<"---------------------------------------------------"<<endl; 
				
	for(i = 0; i < counter_anggota; i++){
	   	cari_id_ambil_sukarela(temp_id, i, &ketemu_id, &posisi_ketemu);  
		   
		if(ketemu_id == 1)
			break; 	 
	}
	
	if(ketemu_id){
		cout<<endl;
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(2);
		cout<<"DATA YANG DICARI"<<endl;
		set_text_color(7);
		cout<<"---------------------------------------------------"<<endl;	
	    cout<<"ID Ambil \t: "<<agt[i].ambil_simpanan_sukarela[posisi_ketemu].id_ambil<<endl;
	    cout<<"Tanggal Ambil \t: "<<agt[i].ambil_simpanan_sukarela[posisi_ketemu].tgl_ambil.tgl<<"-"<<agt[i].ambil_simpanan_sukarela[posisi_ketemu].tgl_ambil.bln<<"-"<<agt[i].ambil_simpanan_sukarela[posisi_ketemu].tgl_ambil.thn<<endl;
	    cout<<"Jumlah Ambil \t: Rp. "<<agt[i].ambil_simpanan_sukarela[posisi_ketemu].jml_ambil<<endl;	     	
		cout<<"---------------------------------------------------"<<endl;
		cout<<endl<<endl;
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
	     
	cout<<"Press Any Key to Continue...";
	getch();
}


/*
	Procedure untuk memilih pengambilan
*/
void menu_ambil_simpanan_sukarela(){
	char menu;
	do {
		system("CLS");		
		cout<<"-------------------------------------"<<endl;
		cout<<"       MENU PENGAMBILAN SIMPANAN     "<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"1. Ambil"<<endl;		
		cout<<"2. Lihat Semua Transaksi"<<endl;
		cout<<"3. Lihat Semua Transaksi Berdasarkan ID Anggota"<<endl;
		cout<<"4. Cari Berdasarkan ID Ambil"<<endl;
		cout<<"5. Kembali"<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"Masukkan Pilihan : ";
		cin>>menu;
		
		switch(menu){
			case '1' : 
				ambil_simpanan_sukarela();
				break;
					 
			case '2' : 
				lihat_ambil_sukarela();
				break;
				
			case '3' : 
				lihat_ambil_sukarela_anggota();
				break;
				
			case '4' : 
				lihat_ambil_sukarela_idambil();
				break;
				
			case '5' : 
				break;
			
			default :
				set_text_color(12);
				cout<<"MENU TIDAK ADA!";
				set_text_color(7);
				fflush(stdin);
				getch();
		}
	} while(menu != '5');
}

/*
	Procedure untuk memilih jenis simpanan
*/
void menu_simpanan_sukarela(){
	char menu;
	do {
		system("CLS");		
		cout<<"-------------------------------------"<<endl;
		cout<<"             MENU SIMPANAN           "<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"1. Simpan"<<endl;		
		cout<<"2. Lihat Semua Transaksi"<<endl;
		cout<<"3. Lihat Semua Transaksi Berdasarkan ID Anggota"<<endl;
		cout<<"4. Cari Berdasarkan ID Simpanan"<<endl;
		cout<<"5. Ambil Simpanan"<<endl;
		cout<<"6. Kembali"<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"Masukkan Pilihan : ";
		cin>>menu;
		
		switch(menu){
			case '1' : 
				simpanan_sukarela();
				break;
					 
			case '2' : 
				lihat_simpanan_sukarela();
				break;
				
			case '3' : 
				lihat_simpanan_sukarela_anggota();
				break;
				
			case '4' : 
				lihat_simpanan_sukarela_idsimpanan();
				break;
				
			case '5' : 
				menu_ambil_simpanan_sukarela();
				break;
				
			case '6' : 
				break;
			
			default :
				set_text_color(12);
				cout<<"MENU TIDAK ADA!";
				set_text_color(7);
				fflush(stdin);
				getch();
		}
	} while(menu != '6');
}

/*
	Procedure untuk menu simpanan
*/
void menu_simpanan(){
	char menu;
	do {
		system("CLS");		
		cout<<"-------------------------------------"<<endl;
		cout<<"             MENU SIMPANAN           "<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"1. Simpanan Wajib"<<endl;
		cout<<"2. Simpanan Sukarela"<<endl;
		cout<<"3. Kembali"<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"Masukkan Pilihan : ";
		cin>>menu;
		
		switch(menu){
			case '1' : 
				menu_simpanan_wajib();
				break;
					 
			case '2' : 
				menu_simpanan_sukarela();
				break;
			
			case '3' :
				break;
			
			default :
				set_text_color(12);
				cout<<"MENU TIDAK ADA!";
				set_text_color(7);
				fflush(stdin);
				getch();
		}
	} while(menu != '3');
}

/*
	Procedure untuk pinjaman
*/
void pinjam(){
	int cari_id;
	int temp_id;
	int i = 0;
	bool ketemu = false;
	bool ketemu_id = false;
	int posisi_ketemu;
	long int temp_pinjaman = 0;
	
	//Tanggal sekarang
	int tgl, bln, thn;
	date_now(&tgl, &bln, &thn);	
	
	system("CLS");
	cout<<"---------------------------------------------------"<<endl;
	cout<<"                    FORM PINJAMAN                  "<<endl;
	cout<<"---------------------------------------------------"<<endl;
	cout<<"Masukkan ID anggota \t\t\t: ";
	cin>>cari_id;
	cout<<"---------------------------------------------------"<<endl;
	
	cari_id_anggota(cari_id, &ketemu, &i);
	
	if(ketemu){	
		if(cari_status_pinjam(i) == 0){
			//Validasi
			ulang_validasi_jum_pinjaman:
			cout<<"Masukkan Jumlah Pinjaman \t\t: ";
			cin>>temp_pinjaman;
			
			//validasi		
			if(temp_pinjaman > uang_koperasi){
				set_text_color(12);
				cout<<"Uang Di Koperasi Tidak Cukup! : Rp. "<<uang_koperasi;
				set_text_color(7);
				getch();
				cout<<endl;
				goto ulang_validasi_jum_pinjaman;
			}	
			
			if((temp_pinjaman / 2) > agt[i].total_simpanan_sukarela){
				set_text_color(12);
				cout<<"Maksimal Pinjaman Anda (Total Simpanan Sukarela * 2) : Rp. "<<agt[i].total_simpanan_sukarela * 2<<endl;
				set_text_color(7);
				cout<<"---------------------------------------------------"<<endl;
			}else{
				cout<<"Tanggal Pinjaman \t\t\t: "<<tgl<<"-"<<bln<<"-"<<thn<<endl;
				agt[i].pinjam[indeks_pinjaman[i]].tgl_pinjam.tgl = tgl;
				agt[i].pinjam[indeks_pinjaman[i]].tgl_pinjam.bln = bln;
				agt[i].pinjam[indeks_pinjaman[i]].tgl_pinjam.thn = thn;
					
				cout<<"Masukkan ID Pinjam \t\t\t: ";
				cin>>temp_id;
					
				//Validasi
				for(int j = 0; j < counter_anggota; j++){
			   		cari_id_pinjam(temp_id, j, &ketemu, &posisi_ketemu);  
							   
					if(ketemu == 1)
						break; 	 	 
				}	 	
				while(ketemu == 1){
				   	set_text_color(12);
				   	cout<<"ID Tidak Boleh Duplikat!";
				   	set_text_color(7);
				   	getch();
				   	
				    cout<<endl<<"ID Pinjam \t\t\t\t: ";
				    cin>>temp_id;
				    
				    for(int j = 0; j < counter_anggota; j++){
				   		cari_id_pinjam(temp_id, j, &ketemu, &posisi_ketemu);  
								   
						if(ketemu == 1)
							break; 	 	 
					}
				}  
				//End Validasi		    
				
				agt[i].pinjam[indeks_pinjaman[i]].id_pinjam = temp_id;
				
				cout<<"Masukkan Jumlah Pinjaman \t\t: Rp. "<<temp_pinjaman<<endl;
				agt[i].pinjam[indeks_pinjaman[i]].jml_pinjam = temp_pinjaman;
				
				ulang_validasi_jum_angsuran:	
				cout<<"Masukkan Jumlah Angsuran [3/6/10] \t: ";
				cin>>agt[i].pinjam[indeks_pinjaman[i]].jml_angsuran_pinjam;
				//validasi
				if(agt[i].pinjam[indeks_pinjaman[i]].jml_angsuran_pinjam != 3 && agt[i].pinjam[indeks_pinjaman[i]].jml_angsuran_pinjam != 6 && agt[i].pinjam[indeks_pinjaman[i]].jml_angsuran_pinjam != 10){
					set_text_color(12);
					cout<<"Hanya Bisa 3/6/10 Bulan!";
					set_text_color(7);
					getch();
					cout<<endl;
					goto ulang_validasi_jum_angsuran;	
				}
				
				cout<<endl;
				cout<<"DETAIL"<<endl;
				cout<<"---------------------------------------------------"<<endl;
				cout<<"Jumlah Pinjaman \t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[indeks_pinjaman[i]].jml_pinjam<<endl;
				
				cout<<"Jumlah Angsuran \t: "<<agt[i].pinjam[indeks_pinjaman[i]].jml_angsuran_pinjam<<endl;
				
				cout<<"Bunga \t\t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[indeks_pinjaman[i]].jml_pinjam * 0.05<<endl;
				agt[i].pinjam[indeks_pinjaman[i]].bunga_pinjam = agt[i].pinjam[indeks_pinjaman[i]].jml_pinjam * 0.05;
				
				cout<<"Total Pinjaman \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[indeks_pinjaman[i]].jml_pinjam + agt[i].pinjam[indeks_pinjaman[i]].bunga_pinjam<<endl;
				agt[i].pinjam[indeks_pinjaman[i]].sisa_pinjam = agt[i].pinjam[indeks_pinjaman[i]].jml_pinjam + agt[i].pinjam[indeks_pinjaman[i]].bunga_pinjam;
				
				cout<<"Cicilan Per Bulan \t: Rp. "<<fixed<<std::setprecision(0)<<(agt[i].pinjam[indeks_pinjaman[i]].jml_pinjam / agt[i].pinjam[indeks_pinjaman[i]].jml_angsuran_pinjam) + (agt[i].pinjam[indeks_pinjaman[i]].bunga_pinjam / agt[i].pinjam[indeks_pinjaman[i]].jml_angsuran_pinjam)<<endl;
				agt[i].pinjam[indeks_pinjaman[i]].cicilan_pinjam = (agt[i].pinjam[indeks_pinjaman[i]].jml_pinjam / agt[i].pinjam[indeks_pinjaman[i]].jml_angsuran_pinjam) + (agt[i].pinjam[indeks_pinjaman[i]].bunga_pinjam / agt[i].pinjam[indeks_pinjaman[i]].jml_angsuran_pinjam);
					
				agt[i].pinjam[indeks_pinjaman[i]].angsuran_ke_pinjam = 0;
				
				//Set ke status sedang pinjam
				agt[i].pinjam[indeks_pinjaman[i]].status_pinjam = 1;
				
				indeks_pinjaman[i] += 1;
				
				keuangan_koperasi();
				
				cout<<endl;
			}
		}else{
			set_text_color(12);
			cout<<"Harap Lunasi Pinjaman Anda Yang Sebelumnya!"<<endl;
			set_text_color(7);
		}
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
	
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk pengembalian pinjaman
*/
void pengembalian_pinjaman(){
	int cari_id;
	int i = 0;
	bool ketemu = false;
	bool ketemu_id = false;
	char lanjut_pembayaran;
	int posisi_ketemu;
	
	system("CLS");
	cout<<"---------------------------------------------------"<<endl;
	cout<<"                    PENGEMBALIAN                  "<<endl;
	cout<<"---------------------------------------------------"<<endl;
	cout<<"Masukkan ID Anggota \t: ";
	cin>>cari_id;
	cout<<"---------------------------------------------------"<<endl;
	
	cari_id_anggota(cari_id, &ketemu, &i);
	
	if (ketemu){
		cout<<"Masukkan ID Pinjaman \t\t: ";
		cin>>cari_id;
		cari_id_pinjam(cari_id, i, &ketemu_id, &posisi_ketemu); 
		 	 
		if(ketemu_id == 1){
			if(agt[i].pinjam[posisi_ketemu].status_pinjam == 1){
				cout<<"Nominal Pengembalian \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[posisi_ketemu].cicilan_pinjam<<endl;
				
				cout<<"---------------------------------------------------"<<endl;
				cout<<"Anda Ingin Melanjukan Pembayaran [Y/T] ?";
				cin>>lanjut_pembayaran;
				cout<<"---------------------------------------------------"<<endl;
				
				if((lanjut_pembayaran == 'Y') || (lanjut_pembayaran == 'y')){
					agt[i].pinjam[posisi_ketemu].angsuran_ke_pinjam += 1;
		   			if(agt[i].pinjam[posisi_ketemu].angsuran_ke_pinjam <= agt[i].pinjam[posisi_ketemu].jml_angsuran_pinjam || agt[i].pinjam[posisi_ketemu].sisa_pinjam >= 0){
						cout<<endl<<"Pemngembalian Angsuran Ke \t: "<<agt[i].pinjam[posisi_ketemu].angsuran_ke_pinjam;
									
						agt[i].pinjam[posisi_ketemu].sisa_pinjam = agt[i].pinjam[posisi_ketemu].sisa_pinjam - agt[i].pinjam[posisi_ketemu].cicilan_pinjam;
					
						cout<<endl<<"Sisa Angsuran \t\t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[posisi_ketemu].sisa_pinjam<<endl;
						
						set_text_color(10);
						cout<<"Pengembalian Angsuran Ke-"<<agt[i].pinjam[posisi_ketemu].angsuran_ke_pinjam<<" Berhasil..."<<endl;
						
						keuangan_koperasi();
						
						set_text_color(7);
					}
					
					//Lunas
					if(agt[i].pinjam[posisi_ketemu].angsuran_ke_pinjam == agt[i].pinjam[posisi_ketemu].jml_angsuran_pinjam){
						set_text_color(10);
						cout<<"Pembayaran Lunas!"<<endl;
						set_text_color(7);
						agt[i].pinjam[posisi_ketemu].status_pinjam = 0;
						
						keuangan_koperasi();
					}
					cout<<endl;
				}
			}
			else{
				set_text_color(12);
				cout<<"Maaf Pinjaman Sudah Lunas!"<<endl;
				set_text_color(7);
			}
		}else{
			set_text_color(12);
			cout<<"Maaf Data Tidak Ditemukan!"<<endl;
			set_text_color(7);
		}	
	}else{
		set_text_color(12);
		cout<<"Maaf Data Tidak Ditemukan!"<<endl;
		set_text_color(7);
	}
	
	cout<<"Press Any Key to Continue...";
	getch();
}

void lihat_daftar_peminjam(){
	int counter = 0;
	system("CLS");
	cout<<"---------------------------------------------------"<<endl;
	cout<<"                    DAFTAR PEMINJAM                "<<endl;
	cout<<"---------------------------------------------------"<<endl<<endl;
	for (int i = 0; i < counter_anggota; i++){
		for(int j = 0; j < indeks_pinjaman[i]; j++){
			if (agt[i].pinjam[j].status_pinjam == 1) {			
				cout<<"---------------------------------------------------"<<endl;
				set_text_color(2);
			    cout<<"DATA KE - "<<counter + 1<<endl;
				set_text_color(7);
				cout<<"---------------------------------------------------"<<endl;
				cout<<"ID Pinjam \t\t: "<<agt[i].pinjam[j].id_pinjam<<endl;
				cout<<"Tanggal Pinjam \t\t: "<<agt[i].pinjam[j].tgl_pinjam.tgl<<"-"<<agt[i].pinjam[j].tgl_pinjam.bln<<"-"<<agt[i].pinjam[j].tgl_pinjam.thn<<endl;
				cout<<"Nama Anggota \t\t: "<<agt[i].nama_anggota<<endl<<endl;
				cout<<"Jumlah Pinjam \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[j].jml_pinjam<<endl;
				cout<<"Jumlah Angsuran Pinjam \t: "<<agt[i].pinjam[j].jml_angsuran_pinjam<<endl;
				cout<<"Bunga Pinjam \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[j].bunga_pinjam<<endl;
				cout<<"Total Pinjam \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[j].jml_pinjam + agt[i].pinjam[j].bunga_pinjam<<endl;
				cout<<"Cicilan Pinjam \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[j].cicilan_pinjam<<endl;
				cout<<"Sisa Pinjam \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[j].sisa_pinjam<<endl;
				cout<<"Angsuran Ke \t\t: "<<agt[i].pinjam[j].angsuran_ke_pinjam<<endl;
				cout<<"---------------------------------------------------"<<endl;
				cout<<endl;
			}
		}
	}
	
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk melihat seluruh transaksi pinjaman
*/
void lihat_riwayat_pinjaman(){
	system("CLS");		
	cout<<"---------------------------------------------------"<<endl; 
	cout<<"                LIHAT RIWAYAT PINJAMAN             "<<endl;
	cout<<"---------------------------------------------------"<<endl; 
		
	cout<<endl;
	for(int i = 0; i < counter_anggota; i++){		
		cout<<"---------------------------------------------------"<<endl;
		set_text_color(2);
	    cout<<"DATA ANGGOTA - "<<agt[i].id_anggota<<" - "<<agt[i].nama_anggota<<endl;
		set_text_color(7);
		
		for(int j = 0; j < indeks_pinjaman[i]; j++){	
			cout<<"---------------------------------------------------"<<endl;
			set_text_color(3);
		    cout<<"DATA KE - "<<j + 1<<endl;
			set_text_color(7);
			cout<<"---------------------------------------------------"<<endl;	
	     	cout<<"ID Pinjam \t\t: "<<agt[i].pinjam[j].id_pinjam<<endl;
	     	cout<<"Tanggal Pinjam \t\t: "<<agt[i].pinjam[j].tgl_pinjam.tgl<<"-"<<agt[i].pinjam[j].tgl_pinjam.bln<<"-"<<agt[i].pinjam[j].tgl_pinjam.thn<<endl;
			cout<<"Nama Anggota \t\t: "<<agt[i].nama_anggota<<endl<<endl;
			cout<<"Jumlah Pinjam \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[j].jml_pinjam<<endl;
			cout<<"Jumlah Angsuran Pinjam \t: "<<agt[i].pinjam[j].jml_angsuran_pinjam<<endl;
			cout<<"Bunga Pinjam \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[j].bunga_pinjam<<endl;
			cout<<"Total Pinjaman \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[j].jml_pinjam + agt[i].pinjam[j].bunga_pinjam<<endl;
			cout<<"Cicilan Pinjam \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[j].cicilan_pinjam<<endl;
			cout<<"Sisa Pinjam \t\t: Rp. "<<fixed<<std::setprecision(0)<<agt[i].pinjam[j].sisa_pinjam<<endl;
			cout<<"Angsuran Ke \t\t: "<<agt[i].pinjam[j].angsuran_ke_pinjam<<endl;
			
			if(agt[i].pinjam[j].status_pinjam == 0)
				cout<<"Status Pinjam \t\t: Lunas"<<endl;
			else
				cout<<"Status Pinjam \t\t: Belum Lunas"<<endl;
			cout<<"---------------------------------------------------"<<endl;
		}
		cout<<endl<<endl;
	}
     
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk menu pinjaman
*/
void menu_pinjaman(){
	char menu;
	do {
		system("CLS");		
		cout<<"-------------------------------------"<<endl;
		cout<<"             MENU PINJAMAN           "<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"1. Pinjaman"<<endl;
		cout<<"2. Pengembalian"<<endl;
		cout<<"3. Daftar Peminjam"<<endl;
		cout<<"4. Lihat Riwayat Peminjam"<<endl;
		cout<<"5. Kembali"<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"Masukkan Pilihan : ";
		cin>>menu;
		
		switch(menu){
			case '1' : 
				pinjam();
				break;
					 
			case '2' : 
				pengembalian_pinjaman();
				break;
			
			case '3' :
				lihat_daftar_peminjam();
				break;
				
			case '4' :
				lihat_riwayat_pinjaman();
				break;
				
			case '5' :
				break;
			
			default :
				set_text_color(12);
				cout<<"MENU TIDAK ADA!";
				set_text_color(7);
				fflush(stdin);
				getch();
		}
	} while(menu != '5');
}

/*
	Procedure keuangan koperasi
*/
void keuangan_koperasi(){
	jum_simpanan_pokok = 0;
	jum_simpanan_wajib = 0;
	jum_simpanan_sukarela = 0;
	jum_ambil_sukarela = 0;
	jum_sisa_pinjam = 0;
	jum_bunga = 0;
	modal_awal = 50000000;
	for(int i = 0; i < counter_anggota; i++){		
		jum_simpanan_pokok += SIMPANAN_POKOK;
		
		for(int j = 0; j < indeks_simpanan_wajib[i]; j++){
			jum_simpanan_wajib += agt[i].simpanan_wajib[j].jml_simpanan;
		}
		
		for(int j = 0; j < indeks_simpanan_sukarela[i]; j++){
			jum_simpanan_sukarela += agt[i].simpanan_sukarela[j].jml_simpanan;
		}
		
		for(int j = 0; j < indeks_ambil_sukarela[i]; j++){
			jum_ambil_sukarela += agt[i].ambil_simpanan_sukarela[j].jml_ambil;
		}
		
		for(int j = 0; j < indeks_pinjaman[i]; j++){
			if(agt[i].pinjam[j].status_pinjam == 1)
				jum_sisa_pinjam += (((agt[i].pinjam[j].sisa_pinjam / (agt[i].pinjam[j].jml_angsuran_pinjam - agt[i].pinjam[j].angsuran_ke_pinjam)) - (agt[i].pinjam[j].bunga_pinjam / agt[i].pinjam[j].jml_angsuran_pinjam)) * (agt[i].pinjam[j].jml_angsuran_pinjam - agt[i].pinjam[j].angsuran_ke_pinjam)); 
			
			if(agt[i].pinjam[j].angsuran_ke_pinjam > 0)
				jum_bunga += (agt[i].pinjam[j].bunga_pinjam / (agt[i].pinjam[j].jml_angsuran_pinjam) * agt[i].pinjam[j].angsuran_ke_pinjam);
		}
	}
		
	uang_koperasi = modal_awal + jum_simpanan_pokok + jum_simpanan_wajib + jum_simpanan_sukarela + jum_bunga - jum_ambil_sukarela - jum_sisa_pinjam;
}

/*
	Procedure menampilkan keuangan koperasi
*/
void tampil_keuangan_koperasi(){
    keuangan_koperasi();	
    
	system("cls");
	cout<<"---------------------------------------------------"<<endl;
	cout<<"             KOPERASI SEJAHTERA SENTOSA           "<<endl;
	cout<<"---------------------------------------------------"<<endl<<endl;
	cout<<"PEMASUKKAN"<<endl;
	cout<<"---------------------------------------------------"<<endl;
	cout<<"Modal Awal Koperasi \t\t\t\t: Rp. "<<modal_awal<<endl;
	cout<<"Jumlah Simpanan Pokok \t\t\t\t: Rp. "<<jum_simpanan_pokok<<endl;
	cout<<"Jumlah Simpanan Wajib \t\t\t\t: Rp. "<<jum_simpanan_wajib<<endl;
	cout<<"Jumlah Simpanan Sukarela (Simpan - Ambil) \t: Rp. "<<jum_simpanan_sukarela - jum_ambil_sukarela<<endl;
	cout<<"Jumlah Bunga Pinjaman Yang Terkumpul \t\t: Rp. "<<fixed<<std::setprecision(0)<<jum_bunga<<endl<<endl;
	cout<<"PENGELUARAN"<<endl;
	cout<<"---------------------------------------------------"<<endl;
	cout<<"Jumlah Sisa Pinjaman \t\t\t\t: Rp. "<<fixed<<std::setprecision(0)<<jum_sisa_pinjam<<endl<<endl;
	cout<<"TOTAL"<<endl;
	cout<<"---------------------------------------------------"<<endl;	
	cout<<"Total Uang Di Koperasi \t\t\t\t: Rp. "<<fixed<<std::setprecision(0)<<uang_koperasi<<endl;
	cout<<endl;
	cout<<"Press Any Key to Continue...";
	getch();
}

/*
	Procedure untuk menu utama
*/
void menu(){
	char menu;
	do {
		system("CLS");
		
		cout<<"-------------------------------------"<<endl;
		cout<<"      KOPERASI SEJAHTERA SENTOSA     "<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"1. Anggota"<<endl;
		cout<<"2. Simpanan"<<endl;
		cout<<"3. Pinjaman"<<endl;
		cout<<"4. Keuangan Koperasi"<<endl;
		cout<<"5. Logout"<<endl;
		cout<<"6. Keluar"<<endl;
		cout<<"-------------------------------------"<<endl;
		cout<<"Masukkan Pilihan : ";
		cin>>menu;

		switch(menu) {
			case '1' : 
				menu_anggota();
				break;
					 
			case '2' : 
				menu_simpanan();
				break;
			
			case '3' : 
				menu_pinjaman();
				break;
				
			case '4' : 
				tampil_keuangan_koperasi();
				break;
				
			case '5' : 
				login();
				break;
				
			case '6' :
				exit(0);
				break;
				
			default :
				set_text_color(12);
				cout<<"MENU TIDAK ADA!";
				set_text_color(7);
				fflush(stdin);
				getch();
		}
	} while(menu != '6');
}

/*
	Procedure untuk login
*/
void login(){
	char char_password;
	string password;
	bool login_status = false;
	
	for(int i = 3; i >= 1; i--){
		password = "";
		system("cls");
		cout<<"-------------------------------"<<endl;
		cout<<"          LOGIN ADMIN          "<<endl;
		cout<<"-------------------------------"<<endl;
	    cout<<"Kesempatan : "<<i<<" Kali"<<endl;
	    cout<<"Password   : ";
	    char_password = getch();
	    
	    //13 == Enter
        while(char_password != 13){
        	//http://www.cplusplus.com/reference/string/string/
        	//8 == Backspace
        	if(char_password == 8){
        		if(password.empty() == false)
        			password.erase(password.length() - 1, 1);
        	}
        	else        	
        		password.push_back(char_password);
        		        		
            char_password = getch();
        }
	
		cout<<endl;
	    if(password == "koperasi"){
	    	menu();
	    	login_status = true;
	    	break;
	    }else{
	    	set_text_color(12);
	    	cout<<"PASSWORD SALAH!";
	    	set_text_color(7);
	    }
	    getch();
	}
	
	if(login_status == false){
		set_text_color(14);
		cout<<endl<<"3X SALAH PASSWORD, APLIKASI AKAN DITUTUP...";
		set_text_color(7);
		getch();
		exit(0);
	}
}

int main(int argc, char** argv) {
	login();
	
	system("PAUSE");
	return 0;
}
