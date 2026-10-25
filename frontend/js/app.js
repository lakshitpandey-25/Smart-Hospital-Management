"use strict";

const CAPACITY = 20;
const patients = [];
let nextOrder = 0;

const form = document.getElementById("patientForm");
const table = document.getElementById("patientTable");
const search = document.getElementById("searchPatient");
const message = document.getElementById("formMessage");

// Display current date
document.getElementById("currentDate").textContent =
    new Date().toLocaleDateString();

// Show form messages
function showMessage(text, isError = false) {
    message.textContent = text;
    message.className = isError ? "error" : "success";
}

// Add safe text cells to the patient table
function addCell(row, value) {
    const cell = document.createElement("td");
    cell.textContent = value;
    row.appendChild(cell);
    return cell;
}

// Display a patient queue
function renderQueue(elementId, items, emptyText) {
    const list = document.getElementById(elementId);
    list.replaceChildren();

    if (items.length === 0) {
        const item = document.createElement("li");
        item.textContent = emptyText;
        list.appendChild(item);
        return;
    }

    items.forEach(patient => {
        const item = document.createElement("li");

        item.textContent =
            `${patient.id} - ${patient.name} (Level ${patient.severity})`;

        list.appendChild(item);
    });
}

// Update patient records, queues and dashboard
function renderPatients() {
    const query = search.value.trim().toLowerCase();

    const filteredPatients = patients.filter(patient =>
        patient.id.toLowerCase().includes(query)
    );

    table.replaceChildren();

    if (filteredPatients.length === 0) {
        const row = document.createElement("tr");
        const cell = addCell(
            row,
            patients.length > 0
                ? "No matching patients found."
                : "No patients registered yet."
        );

        cell.colSpan = 6;
        cell.className = "empty-row";
        table.appendChild(row);
    } else {
        filteredPatients.forEach(patient => {
            const row = document.createElement("tr");

            addCell(row, patient.id);
            addCell(row, patient.name);
            addCell(row, patient.age);
            addCell(row, `Level ${patient.severity}`);
            addCell(row, patient.department);

            const statusCell = addCell(row, "");
            const badge = document.createElement("span");

            badge.className =
                patient.severity <= 2
                    ? "pill emergency"
                    : "pill";

            badge.textContent =
                patient.severity <= 2
                    ? "Priority Queue"
                    : "Waiting";

            statusCell.appendChild(badge);
            table.appendChild(row);
        });
    }

    // Severity 1 is highest priority.
    // Patients with equal severity retain registration order.
    const emergencyPatients = patients
        .filter(patient => patient.severity <= 2)
        .sort((a, b) =>
            a.severity - b.severity || a.order - b.order
        );

    const normalPatients = patients.filter(
        patient => patient.severity > 2
    );

    renderQueue(
        "normalQueue",
        normalPatients,
        "No patients waiting."
    );

    renderQueue(
        "emergencyQueue",
        emergencyPatients,
        "No emergency-priority patients."
    );

    // Update dashboard counts
    document.getElementById("totalPatients").textContent =
        patients.length;

    document.getElementById("waitingPatients").textContent =
        patients.length;

    document.getElementById("emergencyPatients").textContent =
        emergencyPatients.length;

    // Demo capacity only; no beds are assigned automatically.
    document.getElementById("availableBeds").textContent = CAPACITY;
    document.getElementById("freeBeds").textContent = CAPACITY;
    document.getElementById("occupiedBeds").textContent = 0;
}

// Register a new patient
form.addEventListener("submit", function(event) {
    event.preventDefault();

    const id = document.getElementById("patientId").value.trim();
    const name = document.getElementById("patientName").value.trim();
    const ageValue = document.getElementById("patientAge").value;
    const severityValue = document.getElementById("severity").value;
    const department = document.getElementById("department").value;

    const age = Number(ageValue);
    const severity = Number(severityValue);

    // Validate all fields
    if (
        !id ||
        !name ||
        !department ||
        ageValue === "" ||
        !Number.isInteger(age) ||
        age < 0 ||
        age > 120 ||
        ![1, 2, 3, 4, 5].includes(severity)
    ) {
        showMessage("Please enter valid details in all fields.", true);
        return;
    }

    // Prevent duplicate Patient IDs
    const duplicate = patients.some(patient =>
        patient.id.toLowerCase() === id.toLowerCase()
    );

    if (duplicate) {
        showMessage(
            "Patient ID already exists. Please use a unique ID.",
            true
        );
        return;
    }

    // Store patient in temporary demo data
    patients.push({
        id: id,
        name: name,
        age: age,
        severity: severity,
        department: department,
        order: nextOrder++
    });

    form.reset();
    search.value = "";

    renderPatients();
    showMessage("Patient registered successfully.");
});

// Search by Patient ID
search.addEventListener("input", renderPatients);

// Initial dashboard display
renderPatients();