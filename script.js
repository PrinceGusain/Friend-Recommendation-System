function getRecommendations() {
    const list = document.getElementById("recommendationList");
    list.innerHTML = "";

    fetch("output.txt")
        .then(response => {
            if (!response.ok) {
                throw new Error("Failed to load recommendations.");
            }
            return response.text();
        })
        .then(data => {
            const lines = data.trim().split("\n");

            if (lines.length <= 1) {
                const li = document.createElement("li");
                li.textContent = "No recommendations found.";
                list.appendChild(li);
                return;
            }

            // Skip the first line (title)
            for (let i = 1; i < lines.length; i++) {
                const li = document.createElement("li");
                li.textContent = lines[i]; // Already includes name + score
                list.appendChild(li);
            }
        })
        .catch(error => {
            const li = document.createElement("li");
            li.textContent = "Error loading recommendations.";
            list.appendChild(li);
            console.error(error);
        });
}
