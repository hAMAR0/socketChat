function updateChat() {
    $.get('/get_chat', function(data) {
        $('#text').text(data.text);
    });
}
setInterval(updateChat, 1000);
$(document).ready(updateChat);