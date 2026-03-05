/**
 * Phlex Framework - Stakeholder Feedback Survey (Post v0.1.0)
 *
 * HOW TO USE:
 * 1. Go to https://script.google.com and create a new project.
 * 2. Paste the entire contents of this file into the editor (replacing any existing code).
 * 3. Click the "Run" button (or press Ctrl+R / Cmd+R) to run createSurveyForm().
 * 4. Grant the requested permissions when prompted.
 * 5. After the script finishes, open https://docs.google.com/forms to find the newly created form.
 */

function createSurveyForm() {
  var form = FormApp.create('Phlex Survey (v0.1.0)');

  form.setDescription(
    'This survey helps the Phlex development team understand user experiences, ' +
    'prioritize features for upcoming releases, and improve the overall framework design.\n\n' +
    'Estimated completion time: 10–15 minutes.'
  );

  form.setIsQuiz(false);
  form.setCollectEmail(true);
  form.setProgressBar(false);

  // ── Part 1: Respondent Profile ──────────────────────────────────────────────

  form.addSectionHeaderItem()
    .setTitle('Part 1: Respondent Profile');

  // Q1
  form.addMultipleChoiceItem()
    .setTitle('Q1. What is your primary role?')
    .setChoiceValues([
      'Staff Scientist',
      'University Professor',
      'Software Developer',
      'Graduate Student',
      'Postdoc',
    ])
    .showOtherOption(true)
    .setRequired(true);

  // Q2
  form.addScaleItem()
    .setTitle('Q2. How comfortable are you with modern C++?')
    .setBounds(1, 5)
    .setLabels('Not comfortable', 'Very comfortable')
    .setRequired(true);

  // Q3
  form.addScaleItem()
    .setTitle('Q3. How comfortable are you with modern Python?')
    .setBounds(1, 5)
    .setLabels('Not comfortable', 'Very comfortable')
    .setRequired(true);

  // Q4
  form.addParagraphTextItem()
    .setTitle('Q4. What do you plan to use Phlex for (which physics use cases, data-processing techniques, etc.)?');

  // Q5
  form.addScaleItem()
    .setTitle('Q5. How familiar are you with the higher-order function paradigm (transform, fold, unfold, observe)?')
    .setBounds(1, 5)
    .setLabels('Not at all familiar', 'Extremely familiar');

  // ── Part 2: Installation & Setup Experience ─────────────────────────────────

  form.addSectionHeaderItem()
    .setTitle('Part 2: Installation & Setup Experience');

  // Q6
  form.addScaleItem()
    .setTitle('Q6. How easy was it to install and set up Phlex? (Leave blank if not yet installed)')
    .setBounds(1, 5)
    .setLabels('Very Difficult', 'Very Easy');

  // Q7
  form.addMultipleChoiceItem()
    .setTitle('Q7. Which installation method did you use?')
    .setChoiceValues([
      'Spack package manager',
      'Manual build from source',
      'Container (Docker, Podman, etc.)',
      'Pre-built binaries',
      "Haven't installed yet",
    ])
    .showOtherOption(true);

  // Q8
  form.addScaleItem()
    .setTitle('Q8. How clear and helpful was the installation documentation?')
    .setBounds(1, 5)
    .setLabels('Very Unclear', 'Very Clear');

  form.addParagraphTextItem()
    .setTitle('Comments on installation experience');

  // ── Part 3: Core Functionality Assessment ───────────────────────────────────

  form.addSectionHeaderItem()
    .setTitle('Part 3: Core Functionality Assessment');

  // Q9
  form.addCheckboxItem()
    .setTitle('Q9. Which Phlex features have you used? (Select all that apply)')
    .setChoiceValues([
      'C++ transform algorithms',
      'C++ observe algorithms',
      'C++ fold algorithms',
      'C++ unfold algorithms',
      'Python transform algorithms',
      'Python observe algorithms',
      'Data-product providers (C++)',
      'Jsonnet configuration',
      'Data-product handles',
      'Data-layer hierarchy',
      'None yet (planning to)',
    ]);

  // Q10a
  form.addScaleItem()
    .setTitle('Q10a. Rate the ease of registering C++ algorithms.')
    .setBounds(1, 5)
    .setLabels('Very Difficult', 'Very Easy');

  // Q10b
  form.addScaleItem()
    .setTitle('Q10b. Rate the ease of registering Python algorithms.')
    .setBounds(1, 5)
    .setLabels('Very Difficult', 'Very Easy');

  form.addParagraphTextItem()
    .setTitle('Comments on core functionality');

  // ── Part 4: Documentation & Learning Resources ───────────────────────────────

  form.addSectionHeaderItem()
    .setTitle('Part 4: Documentation & Learning Resources');

  // Q11
  form.addScaleItem()
    .setTitle('Q11. Rate the quality of the Phlex README and getting started guide.')
    .setBounds(1, 5)
    .setLabels('Poor', 'Excellent');

  form.addParagraphTextItem()
    .setTitle('Please explain how the README/guide can be improved:');

  // Q12
  form.addScaleItem()
    .setTitle('Q12. How easy was it to get started with the phlex-examples repository?')
    .setBounds(1, 5)
    .setLabels('Very Difficult', 'Very Easy');

  form.addParagraphTextItem()
    .setTitle('Please explain how the phlex-examples repository can be improved:');

  // Q13
  form.addCheckboxItem()
    .setTitle('Q13. What documentation improvements would be most valuable? (Select top 3)')
    .setChoiceValues([
      'More code examples',
      'API reference completeness',
      'Migration guides (from art)',
      'Best practices guide',
    ])
    .showOtherOption(true)
    .setValidation(FormApp.createCheckboxValidation()
      .setHelpText('Please select no more than 3 items.')
      .requireSelectAtMost(3)
      .build());

  form.addParagraphTextItem()
    .setTitle('Comments on documentation');

  // ── Part 5: C++/Python Interoperability ──────────────────────────────────────

  form.addSectionHeaderItem()
    .setTitle('Part 5: C++/Python Interoperability');

  // Q14
  form.addMultipleChoiceItem()
    .setTitle('Q14. Have you used Phlex\'s C++/Python interoperability features?')
    .setChoiceValues([
      'Yes – Python algorithms consuming C++ data products',
      'Yes – C++ algorithms consuming Python data products',
      'Yes – both directions',
      'No, but plan to',
      'No, not needed for my use case',
    ]);

  // Q15
  form.addMultipleChoiceItem()
    .setTitle('Q15. Are there type-conversion or data-marshaling issues between C++ and Python?')
    .setChoiceValues([
      'No issues',
      'Minor issues (workarounds available)',
      'Significant issues (limiting functionality)',
      'Major blockers',
      "Haven't tested this feature",
    ]);

  form.addParagraphTextItem()
    .setTitle('Comments on interoperability');

  // ── Part 6: Missing Features & Pain Points ───────────────────────────────────

  form.addSectionHeaderItem()
    .setTitle('Part 6: Missing Features & Pain Points');

  // Q16
  form.addCheckboxItem()
    .setTitle('Q16. What critical features should be prioritized for future releases? (Select top 3)')
    .setChoiceValues([
      'Ability to create user-defined framework drivers',
      'Ability to filter which data products are processed by algorithms',
      'Better debugging/profiling tools',
      'GPU integration with the framework',
      'Processing non-trivial data layer hierarchies',
      'Safe access to thread-unsafe resources',
      'Support for more Python data types',
      'Support for persistent references and associations',
      'Support for YAML and FHiCL configuration languages',
    ])
    .showOtherOption(true)
    .setValidation(FormApp.createCheckboxValidation()
      .setHelpText('Please select no more than 3 items.')
      .requireSelectAtMost(3)
      .build());

  // Q17
  form.addParagraphTextItem()
    .setTitle('Q17. What is your biggest pain point with Phlex v0.1.0?');

  form.addParagraphTextItem()
    .setTitle('Additional comments on missing features');

  // Q18
  form.addParagraphTextItem()
    .setTitle('Any other comments, suggestions, or feedback?');

  // ── Confirmation ─────────────────────────────────────────────────────────────

  form.setConfirmationMessage(
    'Thank you for your feedback! Your responses are crucial for improving Phlex. ' +
    'The development team will analyze all responses to prioritize work for upcoming releases.'
  );

  Logger.log('Form created: ' + form.getEditUrl());
  Logger.log('Shareable URL: ' + form.getPublishedUrl());
}
