CC = gcc 
WARNING += -Wall -Wextra 
# WARNING += -Wcast-qual -Wcast-align -Wstrict-aliasing -Wpointer-arith -Winit-self -Wshadow -Wswitch-enum -Wstrict-prototypes -Wmissing-prototypes -Wredundant-decls -Wfloat-equal -Wundef -Wvla -Wc++-compat
# TEXT = "VBA, or Visual Basic for Applications in short, is a Microsoft computer programming language which is used to facilitate automation of repetitive Microsoft suite data-processing functions, like generating custom graphs and reports. In this instance, VBA can also be used to automate the process of slides creation in PowerPoint using ChatGPT, an AI that can understand and process both human language and codes."
# TEXT = 'Nhờ những nỗ lực của @dvigovszky, giờ đây bạn có thể triển khai các thành phần @ziglang của mình trên @GolemCloud nguồn mở với khả năng thực thi bền bỉ, minh bạch. ￼ Trích dẫn ￼\n'
TEXT = "hello world"
# TEXT = "Thập kỷ vừa qua đã chứng kiến sự bùng nổ mối quan tâm nghiên cứu và giáo dục về suy luận nhân quả, nhờ những ứng dụng rộng rãi của nó trong nghiên cứu y sinh, khoa học xã hội, trí tuệ nhân tạo, v.v. Cuốn sách giáo khoa này, dựa trên khóa học của tác giả về suy luận nhân quả tại UC Berkeley, đã được giảng dạy trên bảy năm qua, chỉ yêu cầu kiến thức cơ bản về lý thuyết xác suất, suy luận thống kê và hồi quy tuyến tính và logistic. Nó giả định kiến thức tối thiểu về suy luận nhân quả và xem xét xác suất và thống kê cơ bản trong phần phụ lục. Nó bao gồm suy luận nhân quả từ góc độ thống kê và bao gồm các ví dụ và ứng dụng từ thống kê sinh học và kinh tế lượng."

LINKLIBS +=	-lvlc -ljansson -lcurl -lpthread -fopenmp
INCLUDED +=	./lib/tts.c ./lib/trans.c ./lib/common.c ./lib/player.c
# INCLUDED +=	./lib/tts.c ./lib/common.c 

SOURCE = main
SOURCE_CODE = $(SOURCE).c
OUTPFILE += $(SOURCE)

SPEED = 2.0
MODE = 4

clean:
	rm main

debug:
	$(CC) -g $(SOURCE_CODE) $(INCLUDED) -o $(OUTPFILE) $(LINKLIBS) $(WARNING)

release:
	$(CC) -O3 $(SOURCE_CODE) $(INCLUDED) -o $(OUTPFILE) $(LINKLIBS) -DNDEBUG

run: debug
	./main $(SPEED) $(MODE) $(TEXT)

test:
	./main $(SPEED) $(MODE) $(TEXT)
	
test-thread:
	./thread $(SPEED) $(MODE) $(TEXT)

test-omp:
	./omp $(SPEED) $(MODE) $(TEXT)
