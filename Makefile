CC = gcc 
WARNING += -Wall -Wextra 
# WARNING += -Wcast-qual -Wcast-align -Wstrict-aliasing -Wpointer-arith -Winit-self -Wshadow -Wswitch-enum -Wstrict-prototypes -Wmissing-prototypes -Wredundant-decls -Wfloat-equal -Wundef -Wvla -Wc++-compat
TEXT = "VBA, or Visual Basic for Applications in short, is a Microsoft computer programming language which is used to facilitate automation of repetitive Microsoft suite data-processing functions, like generating custom graphs and reports. In this instance, VBA can also be used to automate the process of slides creation in PowerPoint using ChatGPT, an AI that can understand and process both human language and codes."
# TEXT = "Một vài năm trước, tôi đã viết blog về cách tiếp cận của mình đối với việc đóng hộp cát các ứng dụng ít tin cậy hơn mà tôi phải hoặc muốn chạy trên máy chính của mình. Cách tiếp cận đã thay đổi kể từ đó, vì vậy đã đến lúc phải cập nhật. Theo thời gian, tôi ngày càng thất vọng với Firejail: cấu hình thường xuyên bị hỏng khi cập nhật và việc gỡ lỗi cấu hình Firejail là cực kỳ khó khăn. Khi xem xét tất cả các tệp được bao gồm, chúng ta đang nói về hàng trăm dòng cấu hình với sự tương tác tinh tế giữa danh sách cho phép và danh sách chặn. Ngay cả khi tôi biết mình muốn cấp quyền truy cập vào thư mục nào, việc đảm bảo rằng quyền truy cập này thực sự có thể thực hiện được thường là điều không hề nhỏ. Thay vào đó, bây giờ tôi đang sử dụng kết hợp hai cách tiếp cận khác nhau: Flatpak và BubbleBox. Flatpak Hộp cát dễ bảo trì nhất là hộp cát do người khác bảo trì. Vì vậy, khi có Flatpak cho phần mềm mà tôi muốn hoặc phải sử dụng, chẳng hạn như Signal hoặc Zoom, đó thường là cách tiếp cận ưa thích của tôi. Thật không may, Flatpaks có thể đi kèm với các cấu hình mặc định cực kỳ tự do khiến hộp cát gần như vô nghĩa. Các phần ghi đè toàn cục sau đây giúp đảm bảo rằng điều này không xảy ra: [Context] sockets=!gpg-agent;!pcsc;!ssh-auth;!system-bus;!session-bus filesystems=~/.XCompose:ro;xdg- config/fontconfig:ro;!~/.gnupg;!~/.ssh;!xdg-documents;!home;!host [Chính sách xe buýt phiên] org.freedesktop.Flatpak=none org.freedesktop.secrets=none BubbleBox Tuy nhiên, không phải tất cả phần mềm đều tồn tại dưới dạng Flatpak. Ngoài ra, đôi khi tôi muốn phần mềm về cơ bản chạy trên hệ thống máy chủ của tôi (tức là sử dụng /usr thông thường), mà không cần truy cập vào mọi thứ trong thư mục chính của tôi. Ví dụ về điều này là Factorio và VSCodium. Cái sau không hoạt động trong Flatpak vì tôi muốn sử dụng nó với LaTeX và trên thực tế, điều này có nghĩa là nó cần chạy LaTeX được cài đặt thông qua apt. Khuyến nghị chính thức là vô hiệu hóa hộp cát Flatpak một cách hiệu quả, nhưng điều đó hoàn toàn không đúng, vì vậy tôi đã tìm kiếm giải pháp thay thế. bubblewrap cung cấp một giải pháp rất thuận tiện: nó có thể khởi động một ứng dụng trong không gian tên hệ thống tệp riêng của nó với toàn quyền kiểm soát phần nào của hệ thống tệp máy chủ có thể truy cập được từ bên trong hộp cát. Tôi đã viết một trình bao bọc nhỏ xung quanh bong bóng để làm cho cấu hình này thuận tiện hơn một chút khi viết và quản lý; dự án này được gọi là BubbleBox. Cuối tuần này, cuối cùng tôi cũng đã tìm cách thêm hỗ trợ cho xdg-dbus-proxy để các ứng dụng được đóng hộp cát giờ đây có thể truy cập các chức năng D-Bus cụ thể mà không cần truy cập vào toàn bộ xe buýt (nói chung là không an toàn khi tiếp xúc với ứng dụng được đóng hộp cát) . Điều đó có nghĩa là cuối cùng cũng đến lúc viết blog về dự án này, vì vậy chúng ta bắt đầu - nếu bạn quan tâm, hãy xem BubbleBox; trang dự án giải thích cách bạn có thể sử dụng nó để thiết lập hộp cát của riêng mình.1 Tôi cũng cần lưu ý rằng đây không phải là giải pháp hộp cát dựa trên bong bóng duy nhất. bubblejail khá giống nhau nhưng cung cấp GUI cấu hình và một bộ mặc định tốt cung cấp; đó là một tài nguyên rất hữu ích khi tìm ra các cờ bong bóng phù hợp để làm cho các ứng dụng GUI phức tạp hoạt động bình thường. (Ngẫu nhiên, ban đầu, “bubblejail” cũng là cách tôi gọi kịch bản của riêng mình, nhưng sau đó tôi nhận ra rằng cái tên này đã được"


# LINKLIBS +=	-lvlc -ljansson -lcurl -lpthread
LINKLIBS +=	-ljansson -lcurl -lpthread
# INCLUDED +=	./lib/tts.c ./lib/trans.c ./lib/common.c ./lib/player.c
INCLUDED +=	./lib/tts.c ./lib/trans.c ./lib/common.c 

SOURCE = main
SOURCE_CODE = $(SOURCE).c
OUTPFILE += $(SOURCE)

SPEED = 2.0
MODE = 2

clean:
	rm main

debug:
	$(CC) -g $(SOURCE_CODE) $(INCLUDED) -o $(OUTPFILE) $(LINKLIBS) $(WARNING)

release:
	$(CC) -O3 $(SOURCE_CODE) $(INCLUDED) -o $(OUTPFILE) $(LINKLIBS) -DNDEBUG

run: debug
	./main $(SPEED) $(MODE) $(TEXT)
