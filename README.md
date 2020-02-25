# Announcement

Beberapa file yang harus ada dalam repositori tersebut diantaranya:
* Direktori src yang berisi source code yang anda buat.
* File output yang berisi hasil uji dijkstra algorithm pada data uji.
* Makefile. Buatlah sehingga kompilasi program dapat dilakukan hanya dengan pemanggilan command ’make’ saja.
* File README.md yang berisi:
    * Petunjuk penggunaan program.
    * Pembagian tugas. Sampaikan dalam list pengerjaan untuk setiap mahasiswa. Sebagai contoh: XXXX mengerjakan fungsi YYYY, ZZZZ, dan YYZZ.
    * Laporan pengerjaan, dengan struktur laporan sesuai dengan deskripsi pada bagian sebelumnya.

# KELAS 02 - KELOMPOK 10

## Petunjuk Penggunaan Program
Dalam direktori root lakukan kompilasi program dengan makefile:

* make main
* mpirun -np <jumlah processor> --hostfile mpi_hostfile <nama executable file> <node graph> <output file>

## Pembagian Tugas
* 13517035 - Hilmi Naufal Yafie : Paralel DIjksta, Laporan, Eksplorasi
* 13517128 - Yudy Valentino : Serial Dijkstra, laporan, Eksplorasi

## Laporan Pengerjaan
### Deskripsi Solusi Paralel

Solusi yang kami lakukan dalam mengerjarkan Algoritma Dijkstra secara paralel adalah dengan membuat graph, kemudian dari adjacency matriks akan dibagi secara rata
kestiap proses yang sedang digunakan 




### Analisis Solusi
Berdasarkan solusi yang kami terapkan, tentunya banyak cara yang menghasilkan lebih baik dalam pengujian dan lebih mudah, 
salah satunya paralel yang digunakan dibagian node yang dipecah dan algoritma dijkstra tidak mengalami paralelisasi.
Kemudian pemakaian scatter dan getter dapat mengefisiensikan dalam pengiriman message ke proses lain.


### Hasil Uji
Berikut Merupakan hasil uji yang kami lakukan untuk node 100, 500, 1000, dan 3000 baik untuk Serial Dijkstra dan Paralel Dijkstra (dalam seconds):

* **Serial Dijkstra**

| N             | Percobaan 1   | Percobaan 2 | Percobaan 3 | Rata-rata           |
| ------------- |:-------------:| -----:| ------------- |:-------------:|
| 100      | 0,053314 | 0,048412 | 0,065642      | 0.05578933333 |
| 500      | 29,113564      |   29,850699 | 26,956003      | 28.6400886667 |
| 1000 | 13,687153      |    14,105954 | 14,327809      | 14,040305 |
| 3000      | 800,615266 | $1600 | col 3 is      | right-aligned |


* **Paralel Dijkstra**

| N             | Percobaan 1   | Percobaan 2 | Percobaan 3 | Rata-rata           |
| ------------- |:-------------:| -----:| ------------- |:-------------:|
| 100      | 508,32865925 | 1007,96928275 | 786,15209475      | 767.483345583 |
| 500      | 563,04427      |   484,21250025 | 1253,5861415      | 766.94763725 |
| 1000 | are neat      |    $1 | col 3 is      | right-aligned |
| 3000      | right-aligned | $1600 | col 3 is      | right-aligned |



### Analisis Uji
Berdasarkan hasil uji, paralel dijkstra menghasilkan rata-rata waktu yang lebih lama, dibandingkan dengan yang serial.
Dimana seharusnya paralel dijkstra menghasilkan waktu yang lebih cepat dibandingkan dengan serial dikarenakan menggunakan beberapa prosesor untuk membagi pekerjaan sehingga beban yang didapat lebih kecil
